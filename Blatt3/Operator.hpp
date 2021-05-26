//
// Created by tim on 25.05.21.
//

#ifndef BLATT3_OPERATOR_HPP
#define BLATT3_OPERATOR_HPP

#include <vector>
#include <string>
#include <ostream>
#include <set>
#include "VariablePredicate.hpp"
#include "State.hpp"

class Operator {
public:
    using PredList = State::PredList;
    using PossiblePreconditions = std::vector<PredList>;
    Operator(std::string name, PredList preconditions, PredList effects);
    void applySubstitution(const VariablePredicate::Substitution &substitution);
    [[nodiscard]] auto variableNames() const -> std::set<std::string>;
    [[nodiscard]] auto makeApplicable(const State &state, bool allowDoubleSubstitution = true) const
        -> std::vector<Operator>;
    [[nodiscard]] auto getPreconditions() const -> const PredList &;
    [[nodiscard]] auto getEffects() const -> const PredList &;
    [[nodiscard]] bool isAtomic() const;
    [[nodiscard]] State applyTo(const State &state) const;
    [[nodiscard]] bool applicableTo(const State &state) const;

private:
    std::string name;
    PredList preconditions;
    PredList effects;
    friend std::ostream &operator<<(std::ostream &out, const Operator &o);

    static void
    makeApplicable(const Operator::PossiblePreconditions &precondPossibilities, const Operator &op, std::size_t i,
                   std::vector<Operator> &instantiations, const std::set<std::string> &substitutedValues,
                   bool allowDoubleSubstitution);
};

std::ostream &operator<<(std::ostream &out, const Operator &o);


#endif //BLATT3_OPERATOR_HPP
