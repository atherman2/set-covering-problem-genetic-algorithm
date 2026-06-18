#include "population.hpp"
#include "constants.hpp"
#include "util.hpp"

SCPSolution initial_population(std::vector<SCPSolution> &population, SCPInstance &instance) {

}

SCPSolution random_solution(SCPInstance& instance) {
	SCPSolution sol = empty_solution(instance);
	auto unused_columns = instance.columns_range;
	while (sol.uncovered_rows.size() > 0) {

	}
}

void semi_greed_add_column(std::vector<int> unused_columns, SCPSolution &solution, SCPInstance &instance) {

}

std::vector<columnEntry> build_columns_rcl(std::vector<int> unused_columns, SCPSolution &solution, SCPInstance &instance) {
	std::vector<columnEntry> rcl;
	rcl.reserve(COL_RCL_SIZE);
	for (int i = 0; i < unused_columns.size(); i++) {
		if (i < COL_RCL_SIZE)
			i++; //TODO
	}
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
