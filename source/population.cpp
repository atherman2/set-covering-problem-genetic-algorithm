#include "population.hpp"
#include "constants.hpp"

SCPSolution initial_population(std::vector<SCPSolution> &population, SCPInstance &instance) {

}

SCPSolution random_solution(SCPInstance& instance) {
	SCPSolution sol = empty_solution(SCPInstance instance);
	while (sol.uncovered_rows.size() > 0) {

	}
}

std::pair worst
