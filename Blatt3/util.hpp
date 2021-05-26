//
// Created by tim on 25.05.21.
//

#ifndef BLATT3_UTIL_HPP
#define BLATT3_UTIL_HPP
#include <ostream>
#include <string>

namespace util {

    enum class BraceType {
        Par, Bracket, Brace
    };
    template<typename LIST>
    void printList(std::ostream &out, const LIST &list, BraceType braceType = BraceType::Brace) {
        switch (braceType) {
            case BraceType::Par:
                out << "(";
                break;
            case BraceType::Bracket:
                out << "[";
                break;
            case BraceType::Brace:
                out << "{";
                break;
        }

        std::size_t i = 0;
        for (const auto &elem : list) {
            out << elem;
            if (i++ < list.size() - 1) {
                out << ", ";
            }
        }

        switch (braceType) {
            case BraceType::Par:
                out << ")";
                break;
            case BraceType::Bracket:
                out << "]";
                break;
            case BraceType::Brace:
                out << "}";
                break;
        }
    }
}

#endif //BLATT3_UTIL_HPP
