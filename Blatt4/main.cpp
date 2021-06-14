#include <iostream>
#include <unordered_map>
#include <string>
#include <vector>
#include <sstream>
#include <cassert>
#include <algorithm>
#include <fstream>
#include <memory>
#include <unordered_set>

constexpr auto NEG_PREFIX = "nicht";
using PredList = std::unordered_map<std::string, bool>;
namespace util {
    auto splitString(const std::string &string, char delimiter) -> std::vector<std::string> {
        std::vector<std::string> ret;
        std::stringstream tmp(string);
        std::string part;
        while (std::getline(tmp, part, delimiter)) {
            ret.emplace_back(std::move(part));
        }

        return ret;
    }

    bool startsWith(const std::string &s, const std::string &start) {
        return s.rfind(start, 0) == 0;
    }

    std::string makeNegative(const std::string &s) {
        if (!startsWith(s, NEG_PREFIX)) {
            return NEG_PREFIX + s;
        }

        return s;
    }

    void appendPredicates(const std::string &predString, PredList &predicates, bool truthVal) {
        auto preds = splitString(predString, ',');
        for (const auto &p : preds) {
            predicates[p] = truthVal;
        }
    }

    auto getPosAndNeg(const PredList &predicates) -> std::pair<std::vector<std::string>, std::vector<std::string>> {
        std::vector<std::string> pos;
        std::vector<std::string> neg;
        for (const auto &p : predicates) {
            if (p.second) {
                pos.emplace_back(p.first);
            } else {
                neg.emplace_back(p.first);
            }
        }

        return {std::move(pos), std::move(neg)};
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


class State {
public:
    explicit State(PredList preds) : predicates(std::move(preds)) {}
    explicit State(const std::string &spec) {
        auto posNegList = util::splitString(spec, ';');
        if (posNegList.size() == 1) {
            posNegList.emplace_back("");
        }

        assert(posNegList.size() == 2);
        const auto &pos = posNegList.front();
        const auto &neg = posNegList.back();
        util::appendPredicates(pos, predicates, true);
        util::appendPredicates(neg, predicates, false);
    }

    State(PredList preds, std::string actionSequence) : predicates(std::move(preds)),
                                                        actionSequence(std::move(actionSequence)) {}

    auto getPredicates() const -> const PredList & {
        return predicates;
    }

    bool isSolutionOf(const State &state) const {
        const auto &oPreds = state.getPredicates();
        for (auto it = oPreds.cbegin(); it != predicates.end(); ++it) {
            auto res = predicates.find(it->first);
            assert(res != predicates.end());
            if (it->second != res->second) {
                return false;
            }
        }

        return true;
    }

    auto getActionSequence() const -> const std::string & {
        return actionSequence;
    }

    bool operator==(const State &other) const {
        return predicates == other.getPredicates();
    }

private:
    PredList predicates;
    std::string actionSequence;
};

class Action {
public:
    explicit Action(const std::string &spec) {
        auto specParts = util::splitString(spec, ';');
        if (specParts.size() == 4) {
            specParts.emplace_back("");
        }

        assert(specParts.size() == 5);
        name = std::move(specParts.front());
        util::appendPredicates(specParts[1], preconditions, true);
        util::appendPredicates(specParts[2], preconditions, false);
        util::appendPredicates(specParts[3], effects, true);
        util::appendPredicates(specParts[4], effects, false);
    }

    Action(std::string name, PredList preconditions, PredList effects) : name(std::move(name)),
        preconditions(std::move(preconditions)), effects(std::move(effects)) {}

    auto getName() const -> const std::string & {
        return name;
    }

    auto getPreconditions() const -> const PredList & {
        return preconditions;
    }

    auto getEffects() const -> const PredList & {
        return effects;
    }

    bool applicable(const State &state) const {
        const auto &statePreds = state.getPredicates();
        for (const auto & precondition : preconditions) {
            auto stateIt = statePreds.find(precondition.first);
            assert(stateIt != statePreds.end());
            if (precondition.second != stateIt->second) {
                return false;
            }
        }

        return true;
    }

    State applyTo(const State &state) const {
        assert(applicable(state));
        PredList preds = state.getPredicates();
        auto actionSeq = state.getActionSequence();
        if (!actionSeq.empty()) {
            actionSeq.append(",");
        }

        actionSeq.append(name);
        for (const auto &effect : effects) {
            preds[effect.first] = effect.second;
        }

        return State(std::move(preds), std::move(actionSeq));
    }

    Action makePositive(const std::unordered_set<std::string> &negatives) const {
        PredList newPre;
        for (const auto &p : preconditions) {
            if (!p.second) {
                newPre.emplace(util::makeNegative(p.first), true);
            } else {
                newPre.emplace(p);
            }
        }

        PredList newEff;
        for (const auto &e : effects) {
            if (negatives.find(e.first) != negatives.end()) {
                std::string negPred = util::makeNegative(e.first);
                if (e.second) {
                    newEff.emplace(std::move(negPred), false);
                } else {
                    newEff.emplace(std::move(negPred), true);
                }
            }

            newEff.emplace(e);
        }

        return Action(name, std::move(newPre), std::move(newEff));
    }

private:
    std::string name;
    PredList preconditions;
    PredList effects;

};

std::ostream &operator<<(std::ostream &out, const Action &action) {
    out << action.getName() << ";";
    auto [pos, neg] = util::getPosAndNeg(action.getPreconditions());
    std::sort(pos.begin(), pos.end());
    std::sort(neg.begin(), neg.end());
    util::printTo(out, pos);
    out << ";";
    util::printTo(out, neg);
    out << ";";
    std::tie(pos, neg) = util::getPosAndNeg(action.getEffects());
    std::sort(pos.begin(), pos.end());
    std::sort(neg.begin(), neg.end());
    util::printTo(out, pos);
    out << ";";
    util::printTo(out, neg);
    return out;
}

std::ostream &operator<<(std::ostream &out, const State &state) {
    auto [pos, neg] = util::getPosAndNeg(state.getPredicates());
    util::printTo(out, pos);
    out << ";";
    util::printTo(out, neg);
    return out;
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
    const std::string actionName = std::move(line);
    std::getline(in, line);
    const State start(line);
    std::getline(in, line);
    const State goal(line);
    std::unique_ptr<Action> desiredAction;
    std::unordered_set<std::string> negPreconditions;
    while (std::getline(in, line)) {
        Action tmp(line);
        auto [_, neg] = util::getPosAndNeg(tmp.getPreconditions());
        for (auto &n : neg) {
            negPreconditions.emplace(std::move(n));
        }

        if (tmp.getName() == actionName) {
            desiredAction = std::make_unique<Action>(std::move(tmp));
        }
    }

    if (nullptr != desiredAction) {
        std::cout << desiredAction->makePositive(negPreconditions) << std::endl;
        return 0;
    }

    std::cerr << actionName << " not in problem spec" << std::endl;
    return 1;
}
