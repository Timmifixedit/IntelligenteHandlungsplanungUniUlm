#include <iostream>
#include <unordered_map>
#include <string>
#include <vector>
#include <sstream>
#include <cassert>
#include <algorithm>
#include <fstream>
#include <deque>

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
    explicit State(PredList preds) : predicates(std::move(preds)) {}
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
        for (const auto & precondition : preconditions) {
            auto stateIt = statePreds.find(precondition.first);
            assert(stateIt != statePreds.end());
            if (precondition.second != stateIt->second) {
                return false;
            }
        }

        return true;
#endif
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

private:
    std::string name;
    PredList preconditions;
    PredList effects;

};

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
    const State start(line);
    line.clear();
    getline(in, line);
    const State target(line);
    line.clear();
    std::vector<Action> actions;
    while (std::getline(in, line)) {
        actions.emplace_back(std::move(line));
    }

    std::deque<State> fringe = {start};
    std::vector<State> visited;
    while (!fringe.empty()) {
        auto current = std::move(fringe.front());
        fringe.pop_front();
        if (current.isSolutionOf(target)) {
            std::cout << current.getActionSequence() << std::endl;
            return 0;
        }

        for(const auto &action : actions) {
            if (action.applicable(current)) {
                State successor = action.applyTo(current);
                auto res = std::find(visited.begin(), visited.end(), successor);
                if (res == visited.end()) {
                    fringe.emplace_back(successor);
                    visited.emplace_back(std::move(successor));
                }
            }
        }
    }

    std::cout << "Unloesbar" << std::endl;
    return 0;
}
