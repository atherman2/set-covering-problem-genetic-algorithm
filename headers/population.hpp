#include "data_structure.hpp"
#include <vector>
#include <utility>

SCPSolution initial_population(std::vector<SCPSolution>& population, SCPInstance& instance);

SCPSolution random_solution(SCPInstance& instance);

std::pair<int, SCPColumn> worst_pair(std::vector<std::pair<int, SCPColumn>>);
