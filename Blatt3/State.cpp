//
// Created by tim on 25.05.21.
//

#include "State.hpp"
#include "util.hpp"
#include <algorithm>

State::State(PredList predicates, std::string actionSeq) : predicates(std::move(predicates)),
    actionSeq(std::move(actionSeq)) {}

auto State::getPredicates() const -> const State::PredList & {
    return predicates;
}

auto State::constants() const -> std::set<std::string> {
    std::set<std::string> ret;
    for (const auto &p : predicates) {
        ret.merge(p.constants());
    }

    return ret;
}

bool State::isAtomic() const {
    return std::all_of(predicates.begin(), predicates.end(), [](const auto &elem) { return elem.isAtomic(); });
}

auto State::getPredicates() -> State::PredList & {
    return predicates;
}

std::ostream &operator<<(std::ostream &out, const State &state) {
    out << "State";
    util::printList(out, state.getPredicates());
    return out;
}

auto State::getActionSequence() const -> const std::string & {
    return actionSeq;
}

bool State::isSolutionOf(const State &state) const {
    return isAtomic() && state.isAtomic() &&
        std::all_of(state.getPredicates().begin(), state.getPredicates().end(), [this] (const auto &pred) {
            auto res = std::find_if(predicates.begin(), predicates.end(), [&pred] (const auto &tPred) {
                return tPred.fullEqual(pred);
            });

            return res != predicates.end();
        });
}

bool State::operator==(const State &other) const {
    return std::all_of(predicates.begin(), predicates.end(), [&other] (const auto &pred) {
        auto res = std::find_if(other.predicates.begin(), other.predicates.end(), [&pred] (const auto &oPred) {
            return oPred.fullEqual(pred);
        });

        return res != other.predicates.end() || !pred.getTruthVal();
    });
}
