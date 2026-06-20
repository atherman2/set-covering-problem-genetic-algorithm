#include "population.hpp"
#include "constants.hpp"
#include "util.hpp"
#include "random_generator.hpp"

SCPSolution fill_initial_population(std::vector<SCPSolution> &population, SCPInstance &instance) {
	auto sol = random_solution(instance);
	population.push_back(sol);
	auto best_sol = sol;
	for (int i = 1, i <= POPULATION_SIZE; i++) {
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
		semi_greedy_add_column(unused_columns, solution, instance);
	}
	return sol;
}

void semi_greedy_add_column(std::vector<int> unused_columns, SCPSolution &solution, SCPInstance &instance) {
	auto rcl = build_columns_rcl(unused_columns, solution, instance);
	auto new_column_rcl_idx = RandomGenerator::inRange(0, COL_RCL_SIZE - 1);
	solution.add_column(rcl.at(new_column_rcl_idx), instance);
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
