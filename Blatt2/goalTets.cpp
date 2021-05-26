#include <iostream>
#include <unordered_map>
#include <string>
#include <vector>
#include <sstream>
#include <cassert>
#include <algorithm>
#include <fstream>

using PredList = std::unordered_map<std::string, bool>;

auto splitString(const std::string &string, char delimiter) -> std::vector<std::string> {
    std::vector<std::string> ret;
    std::stringstream tmp(string);
    std::string part;
    while (std::getline(tmp, part, delimiter)) {
        ret.emplace_back(std::move(part));
    }

    return ret;
}

void appendPredicates(const std::string &predString, PredList &predicates, bool truthVal) {
    auto preds = splitString(predString, ',');
    for (const auto &p : preds) {
        predicates[p] = truthVal;
    }
}

class State {
public:
    explicit State(const std::string &spec) {
        auto posNegList = splitString(spec, ';');
        if (posNegList.size() == 1) {
            posNegList.emplace_back("");
        }

        assert(posNegList.size() == 2);
        const auto &pos = posNegList.front();
        const auto &neg = posNegList.back();
        appendPredicates(pos, predicates, true);
        appendPredicates(neg, predicates, false);
    }

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

private:
    PredList predicates;
};

class Action {
public:
    explicit Action(const std::string &spec) {
        auto specParts = splitString(spec, ';');
        if (specParts.size() == 4) {
            specParts.emplace_back("");
        }

        assert(specParts.size() == 5);
        name = std::move(specParts.front());
        appendPredicates(specParts[1], preconditions, true);
        appendPredicates(specParts[2], preconditions, false);
        appendPredicates(specParts[3], effects, true);
        appendPredicates(specParts[4], effects, false);
    }

    auto getName() const -> const std::string & {
        return name;
    }

    bool applicable(const State &state) const {
#ifdef CPP20
        return std::ranges::all_of(preconditions.begin(), preconditions.end(),
                                   [&state] (const auto &elem) {
            auto it = state.getPredicates().find(elem.first);
            assert(it != state.getPredicates().end());
            return it->second == elem.second;

        });
#else
        const auto &statePreds = state.getPredicates();
        for (auto it = preconditions.cbegin(); it != preconditions.end(); ++it) {
            auto stateIt = statePreds.find(it->first);
            assert(stateIt != statePreds.end());
            if (it->second != stateIt->second) {
                return false;
            }
        }

        return true;
#endif
    }

private:
    std::string name;
    PredList preconditions;
    PredList effects;

};

int main(int argc, char **argv) {
#ifdef DEBUG
    assert(argc == 2);
    std::fstream in(argv[1]);
    assert(in);
#else
    std::istream &in = std::cin;
#endif
    std::string line;
    std::getline(in, line);
    State start(line);
    line.clear();
    getline(in, line);
    State target(line);
    std::cout << (start.isSolutionOf(target) ? "Ja" : "Nein") << std::endl;
    return 0;
}
