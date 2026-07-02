#include "data_structure.hpp"
#include <vector>
#include <utility>
#include <algorithm>

SCPSolution fill_initial_population(std::vector<SCPSolution>& population, SCPInstance& instance);

SCPSolution random_solution(SCPInstance& instance);

using columnEntry = std::pair<float, int>; // <score, column>

void semi_greedy_add_column(std::vector<columnEntry>& unused_entries, SCPSolution& solution, SCPInstance& instance);

std::vector<columnEntry> build_columns_rcl(std::vector<columnEntry>& unused_entries, SCPSolution &solution, SCPInstance &instance);

void remove_entry(std::vector<columnEntry>& entries, int column);

void update_entries(std::vector<columnEntry>& entries, const std::vector<int>& rows_covered, SCPSolution& solution, SCPInstance& instance);

std::vector<bool> columns_to_be_recalculated(const std::vector<int>& rows_covered, SCPInstance& instance);

columnEntry buildEntry(int column, SCPSolution &solution, SCPInstance &instance);

void select_parents(std::vector<SCPSolution>& parents, std::vector<SCPSolution>& population);

bool column_covers_all(int column_idx, std::vector<int>& rows_to_cover, SCPInstance& instance);

void swap_local_search(SCPSolution& solution, SCPInstance& instance);

SCPSolution crossover(SCPSolution& parent_a, SCPSolution& parent_b, SCPInstance& instance);

SCPSolution make_children(std::vector<SCPSolution>& children, std::vector<SCPSolution>& parents, SCPInstance& instance);

SCPSolution best_solution(SCPSolution& sol_a, SCPSolution& sol_b);

SCPSolution mutate_children(std::vector<SCPSolution>& children, SCPInstance& instance);

void elitism(std::vector<SCPSolution>& population, std::vector<SCPSolution>& children);

int worst_in_vector(std::vector<int>& indexes, std::vector<SCPSolution>&);

SCPSolution genetic_algorithm(SCPInstance& instance);

void mutation(SCPSolution& solution, SCPInstance& instance);
