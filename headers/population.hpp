#include "data_structure.hpp"
#include <vector>
#include <utility>
#include <algorithm>

SCPSolution fill_initial_population(std::vector<SCPSolution>& population, SCPInstance& instance);

SCPSolution random_solution(SCPInstance& instance);

void semi_greedy_add_column(std::vector<int> unused_columns, SCPSolution& solution, SCPInstance& instance);

using columnEntry = std::pair<float, int>; // <score, column>

std::vector<columnEntry> build_columns_rcl(std::vector<int> unused_columns, SCPSolution &solution, SCPInstance &instance);

columnEntry buildEntry(int column, SCPSolution &solution, SCPInstance &instance);

void select_parents(std::vector<SCPSolution>& parents, std::vector<SCPSolution>& population);

bool column_covers_all(int column_idx, std::vector<int>& rows_to_cover, SCPInstance& instance);

void swap_local_search(SCPSolution& solution, SCPInstance& instance);

SCPSolution crossover(SCPSolution& parent_a, SCPSolution& parent_b, SCPInstance& instance);

SCPSolution make_children(std::vector<SCPSolution>& children, std::vector<SCPSolution>& parents, SCPInstance& instance);

SCPSolution best_solution(SCPSolution& sol_a, SCPSolution& sol_b);

SCPSolution genetic_algorithm(SCPInstance& instance);
