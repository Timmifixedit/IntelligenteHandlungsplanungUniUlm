#include <iostream>
#include <deque>
#include <vector>
#include "VariablePredicate.hpp"
#include "Operator.hpp"
#include "State.hpp"

int main() {
    const State init({
                             VariablePredicate("On", {"A", "Floor"}),
                             VariablePredicate("On", {"B", "Floor"}),
                             VariablePredicate("On", {"C", "A"}),
                             VariablePredicate("Clear", {"Floor"}),
                             VariablePredicate("Clear", {"B"}),
                             VariablePredicate("Clear", {"C"})
                     });

    const State goal({
                             VariablePredicate("On", {"C", "Floor"}),
                             VariablePredicate("On", {"B", "C"}),
                             VariablePredicate("On", {"A", "B"}),
                     });

    Operator move("Move", {
                          VariablePredicate("On", {"<X>", "<Y>"}),
                          VariablePredicate("Clear", {"<X>"}),
                          VariablePredicate("Clear", {"<Z>"})
                  },
                  {
                          VariablePredicate("On", {"<X>", "<Z>"}),
                          VariablePredicate("Clear", {"<Y>"}),
                          VariablePredicate("Clear", {"Floor"}),
                          VariablePredicate("On", {"<X>", "<Y>"}, false),
                          VariablePredicate("Clear", {"<Z>"}, false)
                  });
    std::cout << init << std::endl;
    std::cout << move << std::endl;
    std::deque<State> fringe = {init};
    std::vector<State> visited;
    while (!fringe.empty()) {
        State current = std::move(fringe.front());
        fringe.pop_front();
        std::cout << "Current " << current << std::endl;
        if (current.isSolutionOf(goal)) {
            std::cout << "Goal reached by sequence" << std::endl;
            std::cout << current.getActionSequence() << std::endl;
            return 0;
        }

        auto actions = move.makeApplicable(current, false);
        std::cout << "Possible actions:" << std::endl;
        for (const auto &action : actions) {
            if (action.applicableTo(current)) {
                State successor = action.applyTo(current);
                auto res = std::find(visited.begin(), visited.end(), successor);
                if (res == visited.end()) {
                    std::cout << action << std::endl;
                    fringe.emplace_back(successor);
                    visited.emplace_back(std::move(successor));
                }
            }
        }

        std::cout << std::endl;
    }

    std::cout << "Unsolvable!" << std::endl;
    return 0;
}
