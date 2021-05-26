//
// Created by tim on 25.05.21.
//

#ifndef BLATT3_STATE_HPP
#define BLATT3_STATE_HPP
#include <vector>
#include <set>
#include <string>
#include <ostream>
#include "VariablePredicate.hpp"

class State {
public:
    using PredList = std::vector<VariablePredicate>;
    explicit State(PredList predicates, std::string predList = "");
    [[nodiscard]] auto getPredicates() const -> const PredList &;
    [[nodiscard]] auto getPredicates() -> PredList &;
    [[nodiscard]] auto constants() const -> std::set<std::string>;
    [[nodiscard]] bool isAtomic() const;
    [[nodiscard]] auto getActionSequence() const -> const std::string &;
    [[nodiscard]] bool isSolutionOf(const State &state) const;
    bool operator==(const State &other) const;
private:
    PredList predicates;
    std::string actionSeq;
};

std::ostream &operator<<(std::ostream &out, const State &state);

#endif //BLATT3_STATE_HPP
