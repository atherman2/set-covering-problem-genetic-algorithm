#include "data_structure.hpp"
#include <vector>
#include <utility>

SCPSolution fill_initial_population(std::vector<SCPSolution>& population, SCPInstance& instance);

SCPSolution random_solution(SCPInstance& instance);

void semi_greedy_add_column(std::vector<int> unused_columns, SCPSolution& solution, SCPInstance& instance);

using columnEntry = std::pair<float, int>; // <score, column>

std::vector<columnEntry> build_columns_rcl(std::vector<int> unused_columns, SCPSolution &solution, SCPInstance &instance);

columnEntry buildEntry(int column, SCPSolution &solution, SCPInstance &instance);
