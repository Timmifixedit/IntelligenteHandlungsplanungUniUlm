//
// Created by tim on 13.06.21.
//
#include <string>
#include <vector>
#include <sstream>
#include <cassert>
#include <unordered_map>
#include <unordered_set>
#include <iostream>
#include <fstream>
#include <memory>

class Action;
using cActionPtr = std::shared_ptr<const Action>;

class Fact {
public:
    struct Hash {
        std::size_t operator()(const Fact &f) const {
            return std::hash<std::string>()(f.getName());
        }
    };


    explicit Fact(std::string name) : name(std::move(name)) {}

    [[nodiscard]] auto getName() const -> const std::string & {
        return name;
    }

    bool operator==(const Fact &other) const {
        return name == other.getName();
    }

private:
    std::string name;
};

using FactSet = std::unordered_set<Fact, Fact::Hash>;
using FactMap = std::unordered_map<Fact, std::vector<cActionPtr>, Fact::Hash>;

namespace util {
    template<typename T>
    struct Identity {
        T &operator()(T &t) const {
            return t;
        }

        const T &operator()(const T &t) const {
            return t;
        }
    };


    template<typename LIST, typename SET,
            typename ELEM_FUN = Identity<std::decay_t<decltype(*std::begin(std::declval<LIST>()))>>>
    auto subsetOf(const LIST &s1, const SET &s2, const ELEM_FUN &elemFun = ELEM_FUN())
        -> decltype(std::begin(s1), std::end(s1), std::end(s2), s2.find(elemFun(*std::begin(s1))), true) {
        for (const auto &elem : s1) {
            if (s2.find(elemFun(elem)) == std::end(s2)) {
                return false;
            }
        }

        return true;
    }

    template<typename LIST, typename SET,
            typename ELEM_FUN = Identity<std::decay_t<decltype(*std::begin(std::declval<LIST>()))>>>
    auto intersectEmpty(const LIST &s1, const SET &s2, const ELEM_FUN &elemFun = ELEM_FUN())
        -> decltype(std::begin(s1), std::end(s1), std::end(s2), s2.find(elemFun(*std::begin(s1))), true) {
        for (const auto &elem : s1) {
            if (s2.find(elemFun(elem)) != s2.end()) {
                return false;
            }
        }

        return true;
    }

    auto splitString(const std::string &string, char delimiter) -> std::vector<std::string> {
        std::vector<std::string> ret;
        std::stringstream tmp(string);
        std::string part;
        while (std::getline(tmp, part, delimiter)) {
            ret.emplace_back(std::move(part));
        }

        return ret;
    }

    FactSet parse(const std::string &spec) {
        auto preds = splitString(spec, ',');
        FactSet ret;
        for (auto &p : preds) {
            ret.emplace(std::move(p));
        }

        return ret;
    }

    template<typename LIST>
    void printTo(std::ostream &out, const LIST &list) {
        auto it = std::begin(list);
        while (it != std::end(list)) {
            out << *it;
            ++it;
            if (it != std::end(list)) {
                out << ",";
            }
        }
    }
}

class Action {
public:
    explicit Action(const std::string &spec) {
        auto specParts = util::splitString(spec, ';');
        if (specParts.size() == 4) {
            specParts.emplace_back("");
        }

        assert(specParts.size() == 5);
        name = std::move(specParts.front());
        preconditions = util::parse(specParts[1]);
        assert(specParts[2].empty());
        effects = {util::parse(specParts[3]), util::parse(specParts[4])};
    }

    Action(std::string name, FactSet preconditions, FactSet add, FactSet del) : name(std::move(name)),
        preconditions(std::move(preconditions)), effects(std::move(add), std::move(del)) {};

    auto getName() const -> const std::string & {
        return name;
    }

    auto getPreconditions() const -> const FactSet & {
        return preconditions;
    }

    auto getEffects() const -> const std::pair<FactSet, FactSet> & {
        return effects;
    }

    static auto noOp(const Fact &f) -> Action {
        auto newName = "NoOp(" + f.getName() + ")";
        return Action(std::move(newName), {f}, {f}, {});
    }

    bool independent(const Action &other) const {
        const auto &[thisAdd, thisDel] = effects;
        const auto &[a2Add, a2Del] = other.getEffects();
        return util::intersectEmpty(this->preconditions, a2Del) && util::intersectEmpty(thisAdd, a2Del) &&
               util::intersectEmpty(other.getPreconditions(), thisDel) && util::intersectEmpty(a2Add, thisDel);

    }


private:
    std::string name;
    FactSet preconditions;
    std::pair<FactSet, FactSet> effects;
};

class FactLayer {
public:
    FactLayer(FactMap facts, std::size_t depth) : facts(std::move(facts)), depth(depth) {}

    [[nodiscard]] auto getFacts() const -> const FactMap & {
        return facts;
    }

    explicit FactLayer(const std::string &spec) : depth(0) {
        auto posNeg = util::splitString(spec, ';');
        // ignoring negative facts
        auto tmp = util::splitString(posNeg.front(), ',');
        facts.reserve(tmp.size());
        for (auto &f : tmp) {
            facts.emplace(Fact(std::move(f)), std::vector<cActionPtr>());
        }
    }

    std::size_t getDepth() const {
        return depth;
    }

    bool independent(const Fact &f1, const Fact &f2) const {
        const auto res1 = facts.find(f1);
        const auto res2 = facts.find(f2);
        assert(res1 != facts.end() && res2 != facts.end());
        const auto &sourceF1 = res1->second;
        const auto &sourceF2 = res2->second;
        if (sourceF1.empty() || sourceF2.empty()) {
            return true;
        }

        for (const auto& a1 : sourceF1) {
            for (const auto& a2 : sourceF2) {
                if (a1->independent(*a2)) {
                    return true;
                }
            }
        }

        return false;
    }

    bool isApplicable(const Action &action) const {
        if (!util::subsetOf(action.getPreconditions(), facts)) {
            return false;
        }

        for (auto p1 = action.getPreconditions().begin(); p1 != action.getPreconditions().end(); ++p1) {
            auto p2 = p1;
            ++p2;
            while (p2 != action.getPreconditions().end()) {
                if (!independent(*p1, *p2)) {
                    return false;
                }

                ++p2;
            }
        }

        return true;
    }

    bool satisfies(const FactLayer &other) const {
        return util::subsetOf(other.getFacts(), facts, [](const auto &a) -> decltype(a.first)& { return a.first; });
    }

    FactLayer next(const std::vector<cActionPtr> &actions) const {
        FactMap resultingFacts;
        for (const auto &action : actions) {
            const auto &addEffects = action->getEffects().first;
            for (const auto &f : addEffects) {
                resultingFacts[f].emplace_back(action);
            }
        }

        return FactLayer(std::move(resultingFacts), depth + 1);
    }

private:
    FactMap facts;
    std::size_t depth;
};

auto getValidActions(const std::vector<cActionPtr> &allActions, const FactLayer &layer) -> std::vector<cActionPtr> {
    std::vector<cActionPtr> ret;
    ret.reserve(allActions.size());
    for (const auto &a : allActions) {
        if (layer.isApplicable(*a)) {
            ret.emplace_back(a);
        }
    }

    for (const auto &f : layer.getFacts()) {
        ret.emplace_back(std::make_shared<Action>(Action::noOp(f.first)));
    }

    return ret;
}


int main(int argc, char **argv) {
#ifdef DEBUG
    assert(argc == 2);
    std::fstream in(argv[1]);
    if (!in) {
        std::cout << argv[1] << std::endl;
    }
    assert(in);
#else
    std::istream &in = std::cin;
#endif
    std::string line;
    std::getline(in, line);
    const FactLayer start(line);
    std::getline(in, line);
    const FactLayer goal(line);
    std::vector<cActionPtr> actions;
    while (std::getline(in, line)) {
        actions.emplace_back(std::make_shared<Action>(line));
    }

    FactLayer current = start;
    while (!current.satisfies(goal)) {
        auto possibleActions = getValidActions(actions, current);
        if (possibleActions.empty()) {
            std::cout << -1 << std::endl;
            return 0;
        }

        current = current.next(possibleActions);
    }

    std::cout << current.getDepth() << std::endl;
    return 0;
}