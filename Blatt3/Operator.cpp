//
// Created by tim on 25.05.21.
//

#include <algorithm>
#include <cassert>
#include <exception>
#include <sstream>
#include <list>
#include "Operator.hpp"
#include "util.hpp"

Operator::Operator(std::string name, Operator::PredList preconditions, Operator::PredList effects) :
        name(std::move(name)), preconditions(std::move(preconditions)), effects(std::move(effects)) {}

void Operator::applySubstitution(const VariablePredicate::Substitution &substitution) {
    for (auto &v : preconditions) {
        v.applySubstitution(substitution);
    }

    for (auto &v : effects) {
        v.applySubstitution(substitution);
    }
}

std::ostream &operator<<(std::ostream &out, const Operator &o) {
    out << o.name;
    util::printList(out, o.variableNames());
    out << ": pre";
    util::printList(out, o.preconditions);
    out << " => eff";
    util::printList(out, o.effects);
    return out;
}

auto Operator::variableNames() const -> std::set<std::string> {
    std::set<std::string> ret;
    auto insertNames = [&ret](const PredList &predList) {
        for (const auto &pred : predList) {
            for (const auto &v : pred.getVariables()) {
                ret.emplace(v);
            }
        }
    };

    insertNames(preconditions);
    insertNames(effects);
    return ret;
}

void
Operator::makeApplicable(const Operator::PossiblePreconditions &precondPossibilities, const Operator &op, std::size_t i,
                         std::vector<Operator> &instantiations, const std::set<std::string> &substitutedValues,
                         bool allowDoubleSubstitution) {

    if (i == op.getPreconditions().size()) {
        instantiations.emplace_back(op);
        return;
    }

    const auto &currReq = op.getPreconditions()[i];
    const auto &currentPossibilities = precondPossibilities[i];
    for (const auto &pred : currentPossibilities) {
        auto subs = findSubstitution(currReq, pred);
        if (subs.has_value()) {
            auto newSubsVals = substitutedValues;
            if (!allowDoubleSubstitution) {
                bool doubleSubstitution = false;
                for (const auto &[oldVal, newVal] : subs->first) {
                    if (newSubsVals.contains(newVal)) {
                        doubleSubstitution = true;
                        break;
                    } else {
                        newSubsVals.emplace(newVal);
                    }
                }

                if (doubleSubstitution) {
                    continue;
                }
            }

            Operator substituted = op;
            substituted.applySubstitution(subs->first);
            makeApplicable(precondPossibilities, substituted, i + 1, instantiations, newSubsVals,
                           allowDoubleSubstitution);
        }
    }
}

auto Operator::makeApplicable(const State &state, bool allowDoubleSubstitution) const -> std::vector<Operator> {
    std::vector<Operator> ret;
    PossiblePreconditions possibleConditions(preconditions.size());
    for (std::size_t i = 0; i < preconditions.size(); ++i) {
        for (const auto &pred : state.getPredicates()) {
            if (pred.typeEqual(preconditions[i]) && pred.getTruthVal() == preconditions[i].getTruthVal()) {
                possibleConditions[i].emplace_back(pred);
            }
        }
    }

    if (std::any_of(possibleConditions.begin(), possibleConditions.end(),
                    [](const auto &set) { return set.empty(); })) {
        return ret;
    }

    makeApplicable(possibleConditions, *this, 0, ret, {}, allowDoubleSubstitution);
    return ret;
}

auto Operator::getPreconditions() const -> const Operator::PredList & {
    return preconditions;
}

auto Operator::getEffects() const -> const Operator::PredList & {
    return effects;
}

bool Operator::isAtomic() const {
    return std::all_of(preconditions.begin(), preconditions.end(),
                       [](const auto &elem) { return elem.isAtomic(); }) &&
           std::all_of(effects.begin(), effects.end(), [](const auto &elem) { return elem.isAtomic(); });
}

State Operator::applyTo(const State &state) const {
    if (!applicableTo(state)) {
        std::stringstream msg;
        msg << "Operator " << *this << std::endl << "is not applicable to state" << state;
        throw std::runtime_error(msg.str());
    }

    auto actionSeq = state.getActionSequence();
    if (!actionSeq.empty()) {
        actionSeq.append("\n");
    }

    std::stringstream opName;
    opName << *this;
    actionSeq.append(opName.str());
    const auto &oldPreds = state.getPredicates();
    PredList preds;
    preds.reserve(oldPreds.size());
    std::list tmpEffects(effects.begin(), effects.end());
    for (const auto &oPred : oldPreds) {
        auto res = std::find_if(tmpEffects.begin(), tmpEffects.end(), [&oPred] (const auto &eff) {
            return eff.atomicEqual(oPred);
        });

        if (res == tmpEffects.end()) {
            preds.emplace_back(oPred);
        } else {
            if (res->getTruthVal() || !oPred.getTruthVal()) {
                preds.emplace_back(std::move(*res));
            }

            tmpEffects.erase(res);
        }
    }

    for (auto &eff : tmpEffects) {
        preds.emplace_back(std::move(eff));
    }

    return State(std::move(preds), std::move(actionSeq));
}

bool Operator::applicableTo(const State &state) const {
    if (!isAtomic() || !state.isAtomic()) {
        return false;
    }

    return std::all_of(preconditions.begin(), preconditions.end(), [&state] (const auto &cond) {
        auto res = std::find_if(state.getPredicates().begin(), state.getPredicates().end(), [&cond] (const auto &pred) {
            return pred.fullEqual(cond);
        });

        return res != state.getPredicates().end();
    });
}
