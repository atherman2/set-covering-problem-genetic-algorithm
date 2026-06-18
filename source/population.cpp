#include "population.hpp"
#include "constants.hpp"

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
