#include "data_structure.hpp"
#include <vector>
#include <utility>

SCPSolution initial_population(std::vector<SCPSolution>& population, SCPInstance& instance);

SCPSolution random_solution(SCPInstance& instance);

void semi_greed_add_column(std::vector<int> unused_columns, SCPSolution& solution, SCPInstance& instance);

using columnEntry = std::pair<float, int>;

std::vector<columnEntry> build_columns_rcl();
