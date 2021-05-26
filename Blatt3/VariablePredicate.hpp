//
// Created by tim on 25.05.21.
//

#ifndef BLATT3_VARIABLEPREDICATE_HPP
#define BLATT3_VARIABLEPREDICATE_HPP

#include <vector>
#include <string>
#include <ostream>
#include <unordered_map>
#include <set>

class VariablePredicate {
public:
    using Substitution = std::unordered_map<std::string, std::string>;

    VariablePredicate(std::string name, std::size_t numVars, bool truethVal = true);
    VariablePredicate(std::string name, std::vector<std::string> variables, bool truthVal = true);

    /**
     * True if this and other are equal in all respective constants and all free variables of this are matched with
     * a free variable in other at the same position
     * @param other
     * @return
     */
    [[nodiscard]] bool partialEqual(const VariablePredicate &other) const;

    /**
     * True if type equal and this and other are atomic and all constants match respectively
     * @param other
     * @return
     */
    [[nodiscard]] bool atomicEqual(const VariablePredicate& other) const;

    /**
     * True if this and other are atomicEqual and truth values are equal
     * @param other
     * @return
     */
    [[nodiscard]] bool fullEqual(const VariablePredicate& other) const;

    /**
     * True if this and other have the same name
     * @param other
     * @return
     */
    [[nodiscard]] bool typeEqual(const VariablePredicate &other) const;

    [[nodiscard]] bool isAtomic() const;

    void applySubstitution(const Substitution &substitution);

    void applySubstitution(const std::string &oldName, const std::string &newName);

    static bool isVariable(const std::string &varName);

    static bool varInequal(const std::string &v1, const std::string &v2);

    [[nodiscard]] auto getVariables() const -> const std::vector<std::string> &;

    [[nodiscard]] auto constants() const -> std::set<std::string>;

    [[nodiscard]] auto getName() const -> const std::string &;

    void setTruthVal(bool newTruthVal);

    [[nodiscard]] bool getTruthVal() const;
private:
    std::string name;
    std::vector<std::string> variables;
    bool truthVal;

    friend std::ostream &operator<<(std::ostream &out, const VariablePredicate &v);
};

std::ostream &operator<<(std::ostream &out, const VariablePredicate &v);

auto findSubstitution(const VariablePredicate &vp1, const VariablePredicate &vp2)
-> std::optional<std::pair<VariablePredicate::Substitution, VariablePredicate::Substitution>>;


#endif //BLATT3_VARIABLEPREDICATE_HPP
