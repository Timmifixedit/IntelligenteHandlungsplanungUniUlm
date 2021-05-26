//
// Created by tim on 25.05.21.
//

#include "VariablePredicate.hpp"
#include "util.hpp"
#include <cassert>

VariablePredicate::VariablePredicate(std::string name, std::size_t numVars, bool truthVal) :
    name(std::move(name)), variables(numVars), truthVal(truthVal) {}

bool VariablePredicate::partialEqual(const VariablePredicate &other) const {
    if (!typeEqual(other)) {
        return false;
    }

    return std::equal(variables.cbegin(), variables.cend(), other.variables.cbegin(),
                      [](const auto &a, const auto &b) {
                          return (isVariable(a) && isVariable(b)) || a == b;
    });
}

bool VariablePredicate::isAtomic() const {
    return std::all_of(variables.cbegin(), variables.cend(), [] (const auto &elem) {
        return !isVariable(elem);
    });
}

VariablePredicate::VariablePredicate(std::string name, std::vector<std::string> variables, bool truthVal) :
    name(std::move(name)), variables(std::move(variables)), truthVal(truthVal) {}

std::ostream &operator<<(std::ostream &out, const VariablePredicate &v) {
    if (!v.truthVal) {
        out << "¬";
    }

    out << v.name << "(";
    util::printList(out, v.variables, util::BraceType::Par);
    return out;
}

bool VariablePredicate::isVariable(const std::string &varName) {
    return varName.starts_with('<') && varName.ends_with('>');
}

void VariablePredicate::applySubstitution(const VariablePredicate::Substitution &substitution) {
    for (auto &v : variables) {
        if (!isVariable(v)) {
            continue;
        }

        auto it = substitution.find(v);
        if (it != substitution.end()) {
            v = it->second;
        }
    }
}

void VariablePredicate::applySubstitution(const std::string &oldName, const std::string &newName) {
    for (auto &v : variables) {
        if (isVariable(v) && v == oldName) {
            v = newName;
        }
    }
}

auto VariablePredicate::getVariables() const -> const std::vector<std::string> & {
    return variables;
}

bool VariablePredicate::typeEqual(const VariablePredicate &other) const {
    return name == other.name && variables.size() == other.variables.size();
}

bool VariablePredicate::varInequal(const std::string &v1, const std::string &v2) {
    return !isVariable(v1) && !isVariable(v2) && v1 != v2;
}

auto VariablePredicate::constants() const -> std::set<std::string> {
    std::set<std::string> ret;
    for (const auto &v : variables) {
        if (!isVariable(v)) {
            ret.emplace(v);
        }
    }

    return ret;
}

auto VariablePredicate::getName() const -> const std::string & {
    return name;
}

void VariablePredicate::setTruthVal(bool newTruthVal) {
    this->truthVal = newTruthVal;
}

bool VariablePredicate::atomicEqual(const VariablePredicate &other) const {
    return typeEqual(other) && isAtomic() && variables == other.variables;
}

bool VariablePredicate::fullEqual(const VariablePredicate &other) const {
    return atomicEqual(other) && truthVal == other.truthVal;
}

bool VariablePredicate::getTruthVal() const {
    return truthVal;
}

static bool findSubstitution(VariablePredicate &v1, VariablePredicate &v2,
                             VariablePredicate::Substitution &s1, VariablePredicate::Substitution &s2) {
    if (v1.partialEqual(v2)) {
        return true;
    }

    auto it1 = v1.getVariables().cbegin();
    auto it2 = v2.getVariables().cbegin();
    while (it1 != v1.getVariables().end()) {
        if (VariablePredicate::varInequal(*it1, *it2)) {
            return false;
        }

        if (VariablePredicate::isVariable(*it1) && !VariablePredicate::isVariable(*it2)) {
            std::string oldVal = *it1;
            v1.applySubstitution(oldVal, *it2);
            s1[oldVal] = *it2;
        }

        ++it1; ++it2;
    }

    return findSubstitution(v2, v1, s2, s1);
}

auto findSubstitution(const VariablePredicate &vp1, const VariablePredicate &vp2)
-> std::optional<std::pair<VariablePredicate::Substitution, VariablePredicate::Substitution>> {
    if (!vp1.typeEqual(vp2)) {
        return {};
    }

    VariablePredicate v1 = vp1;
    VariablePredicate v2 = vp2;
    VariablePredicate::Substitution s1;
    VariablePredicate::Substitution s2;
    if (!findSubstitution(v1, v2, s1, s2)) {
        return {};
    } else {
        return {{std::move(s1), std::move(s2)}};
    }
}
