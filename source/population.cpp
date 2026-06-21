#include "population.hpp"
#include "constants.hpp"
#include "util.hpp"
#include "random_generator.hpp"

SCPSolution fill_initial_population(std::vector<SCPSolution> &population, SCPInstance &instance) {
	auto sol = random_solution(instance);
	population.push_back(sol);
	auto best_sol = sol;
	for (int i = 1; i <= POPULATION_SIZE; i++) {
		auto sol = random_solution(instance);
		population.push_back(sol);
		if (sol.cost < best_sol.cost)
			best_sol = sol;
	}
	return best_sol;
}

SCPSolution random_solution(SCPInstance& instance) {
	SCPSolution sol = empty_solution(instance);
	auto unused_columns = instance.columns_range;
	while (sol.uncovered_rows.size() > 0) {
		semi_greedy_add_column(unused_columns, sol, instance);
	}
	return sol;
}

void semi_greedy_add_column(std::vector<int> unused_columns, SCPSolution &solution, SCPInstance &instance) {
	auto rcl = build_columns_rcl(unused_columns, solution, instance);
	auto new_column_rcl_idx = RandomGenerator::inRange(0, (int)rcl.size() - 1);
	solution.add_column(rcl.at(new_column_rcl_idx).second, instance);
}

std::vector<columnEntry> build_columns_rcl(std::vector<int> unused_columns, SCPSolution &solution, SCPInstance &instance) {
	std::vector<columnEntry> rcl;
	rcl.reserve(COL_RCL_SIZE);
	for (int i = 0; i < unused_columns.size(); i++) {
		auto entry = buildEntry(unused_columns.at(i), solution, instance);
		if (i < COL_RCL_SIZE)
			rcl.emplace_back(entry);
		else {
			auto& smallest = util::smallest_first(rcl);
			if (entry.first > smallest.first) {
				util::remove_by_ref(rcl, smallest);
				rcl.push_back(entry);
			}
		}
	}
	return rcl;
}

columnEntry buildEntry(int column_idx, SCPSolution &solution, SCPInstance &instance) {
	int covered_count = 0;
	SCPColumn column = instance.columns.at(column_idx);
	for (int i = 0; i < solution.uncovered_rows.size(); i++) {
		if (
			util::is_in(
			column.covered_rows,
			solution.uncovered_rows.at(i)
			)
	 	) covered_count++;
	}
	return {covered_count/column.cost, column_idx};
}

/*
- Checks if a given column covers all rows in the provided list.
- Returns true if the column covers every row.
*/
bool column_covers_all(int column_idx, std::vector<int>& rows_to_cover, SCPInstance& instance) {
	auto& column = instance.columns.at(column_idx);

	for (int i = 0; i < rows_to_cover.size(); i++) {
		if (!util::is_in(column.covered_rows, rows_to_cover.at(i)))
			return false;
	}

	return true;
}

/* Local Search Swap
Tries to improve the current solution by replacing columns with cheaper ones.
- For each column in the solution, attempts to remove it.
- If the removal breaks coverage, it looks for a cheaper column that can cover all uncovered rows. If found, it replaces the original; otherwise, it restores it.
*/
void swap_local_search(SCPSolution& solution, SCPInstance& instance) {
	auto columns_to_check = solution.columns_used;

	for (int i = 0; i < columns_to_check.size(); i++) {
		int column_a_idx = columns_to_check.at(i);
		float column_a_cost = instance.columns.at(column_a_idx).cost;

		// Try removing column A
		solution.remove_column(column_a_idx, instance);

		// If solution is still valid, the column was redundant and we keep it removed
		if (solution.uncovered_rows.size() == 0)
			continue;

		// Try to find a better column to replace it
		int best_b_idx = -1;
		float best_b_cost = column_a_cost;

		for (int column_b_idx = 0; column_b_idx < instance.num_columns; column_b_idx++) {
			if (util::is_in(solution.columns_used, column_b_idx))
				continue;	// Skip columns already in the solution

			float column_b_cost = instance.columns.at(column_b_idx).cost;

			if (column_b_cost >= best_b_cost)
				continue;	// Skip if not cheaper than current best
			
			// Check if this column covers all currently uncovered rows
			if (column_covers_all(column_b_idx, solution.uncovered_rows, instance)) {
				best_b_idx = column_b_idx;
				best_b_cost = column_b_cost;
			}
		}

		if (best_b_idx != -1)
			solution.add_column(best_b_idx, instance); // If a better column was found, add it
		else
			solution.add_column(column_a_idx, instance); // Or else, restore the original column
	}
}