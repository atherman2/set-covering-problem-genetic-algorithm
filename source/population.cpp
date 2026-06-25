#include "population.hpp"
#include "constants.hpp"
#include "util.hpp"
#include "random_generator.hpp"
#include <algorithm>
#include <iostream>
#include <vector>

SCPSolution fill_initial_population(std::vector<SCPSolution> &population, SCPInstance &instance) {
	auto sol = random_solution(instance);
	population.push_back(sol);
	auto best_sol = sol;
	for (int i = 2; i <= POPULATION_SIZE; i++) {
		std::cout << i << "%," << std::endl;
		auto sol = random_solution(instance);
		population.push_back(sol);
		if (sol.cost < best_sol.cost)
			best_sol = sol;
	}
	std::cout << "\n";
	return best_sol;
}

SCPSolution random_solution(SCPInstance& instance) {
	SCPSolution sol = empty_solution(instance);
	auto unused_columns = instance.columns_range;
	int i = 1;
	while (sol.uncovered_rows.size() > 0) {
		// std::cout << i << " ";
		i++;
		semi_greedy_add_column(unused_columns, sol, instance);
	}
	// std::cout << "\n";
	return sol;
}

void semi_greedy_add_column(std::vector<int>& unused_columns, SCPSolution &solution, SCPInstance &instance) {
	auto rcl = build_columns_rcl(unused_columns, solution, instance);
	auto new_column_rcl_idx = RandomGenerator::inRange(0, (int)rcl.size() - 1);
	solution.add_column(rcl.at(new_column_rcl_idx).second, instance);
	util::remove(unused_columns, rcl.at(new_column_rcl_idx).second);
}

std::vector<columnEntry> build_columns_rcl(std::vector<int>& unused_columns, SCPSolution &solution, SCPInstance &instance) {
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

void select_parents(std::vector<SCPSolution>& parents, std::vector<SCPSolution>& population) {
    while(parents.size() < (2 * PARENT_PAIRS_COUNT)) {
        auto tournament_participants_idx = RandomGenerator::uniqueRangeSet(0, POPULATION_SIZE - 1, 3);
        std::vector<SCPSolution> tournament_participants;
        for (int i = 0; i < tournament_participants_idx.size(); i++)
            tournament_participants.push_back(population.at(tournament_participants_idx.at(i)));
        auto best = tournament_participants.at(0);
        for (int i = 1; i < tournament_participants.size(); i++)
            if (tournament_participants.at(i).cost < best.cost)
                best = tournament_participants.at(i);
        if (!util::ref_is_in(parents, best))
            parents.push_back(best);
    }
}

/**
 * Checks if a given column covers all rows in the provided list.
 * Returns true if the column covers every row.
 */
bool column_covers_all(int column_idx, std::vector<int>& rows_to_cover, SCPInstance& instance) {
	auto& column = instance.columns.at(column_idx);

	for (int i = 0; i < rows_to_cover.size(); i++) {
		if (!util::is_in(column.covered_rows, rows_to_cover.at(i)))
			return false;
	}

	return true;
}

/** Local Search Swap
 * Tries to improve the current solution by replacing columns with cheaper ones.
 * For each column in the solution, attempts to remove it.
 * If the removal breaks coverage, it looks for a cheaper column that can cover all uncovered rows. If found, it replaces the original; otherwise, it restores it.
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

/**
 * Fusion crossover
 * Merges both parents, sorts columns by their cost/benefit ratio (worst first), and drops redundant columns to minimize the final solution cost
 */
SCPSolution crossover(SCPSolution& parent_a, SCPSolution& parent_b, SCPInstance& instance) {
    SCPSolution child = empty_solution(instance);

    // Combine all columns from parent A and B into the child
    for (int col : parent_a.columns_used)
        child.add_column(col, instance);

    for (int col : parent_b.columns_used)
        if (!util::is_in(child.columns_used, col))
            child.add_column(col, instance);

    // Sort columns by cost-efficiency ratio (cost / total rows covered), desceding order to target and remove the worst columns first
    std::sort(child.columns_used.begin(), child.columns_used.end(),
    [&instance](int a, int b) {
        float score_a = instance.columns.at(a).cost / (float)instance.columns.at(a).covered_rows.size();
        float score_b = instance.columns.at(b).cost / (float)instance.columns.at(b).covered_rows.size();
        return score_a > score_b;
    });

    // Create a static copy of the columns to safely iterate through
    auto columns_to_check = child.columns_used;
    for (int col : columns_to_check) {
        child.remove_column(col, instance);

        // If removing the column leaves any row uncovered, restore it
        if (child.uncovered_rows.size() > 0)
            child.add_column(col, instance);
    }

    return child;
}

SCPSolution make_children(std::vector<SCPSolution>& children, std::vector<SCPSolution>& parents, SCPInstance& instance) {
    auto best_sol = crossover(parents.at(0), parents.at(1), instance);
    for (int i = 1; (2 * i + 1) < parents.size(); i++) {
        auto child = crossover(parents.at(2 * i), parents.at(2 * i + 1), instance);
        if (child.cost < best_sol.cost)
            best_sol = child;
        children.push_back(child);
    }
    return best_sol;
}

SCPSolution best_solution(SCPSolution& sol_a, SCPSolution& sol_b) {
    if (sol_a.cost <= sol_b.cost) {
        return sol_a;
    }
    return sol_b;
}

SCPSolution mutate_children(std::vector<SCPSolution>& children, SCPInstance& instance) {
	int mutation_count = std::max((int) (POPULATION_SIZE * MUTATION_RATE), 1);
	auto selected_children_indexes = RandomGenerator::uniqueRangeSet(0, children.size(), mutation_count);
	for (int i = 0; i < selected_children_indexes.size(); i++) {
		mutation(children.at(selected_children_indexes.at(i)), instance);
	}
	auto best_sol = children.at(selected_children_indexes.at(0));
	for (int i = 1; i < selected_children_indexes.size(); i++) {
		if (children.at(selected_children_indexes.at(i)).cost < best_sol.cost)
			best_sol = children.at(selected_children_indexes.at(i));
	}
	return best_sol;
}

SCPSolution genetic_algorithm(SCPInstance& instance) {
    int current_generation = 1;
    std::vector<SCPSolution> population, parents, children;
    SCPSolution best_known_solution = fill_initial_population(population, instance);
    while (current_generation <= MAX_GENERATION) {
        select_parents(parents, population);
        auto best_child = make_children(children, parents, instance);
        best_known_solution = best_solution(best_known_solution, best_child);
        auto best_mutated_child = mutate_children(children, instance);
        best_known_solution = best_solution(best_known_solution, best_mutated_child);
        current_generation++;
    }
    return best_known_solution;
}

/**
 * Mutation operator
 * Removes k (defined in constants.hpp) random columns from the solution.
 * Repairs by randomly picking columns that cover uncovered rows.
 * If a row has no available column to cover it, restores a removed column as fallback.
 */
void mutation(SCPSolution& solution, SCPInstance& instance) {
    if (solution.columns_used.empty()) return;

    // Stores removed columns in case we need them for fallback
    std::vector<int> removed_cols;
    for (int i = 0; i < MUTATION_K && !solution.columns_used.empty(); i++) {
        int random_idx = RandomGenerator::inRange(0, (int)solution.columns_used.size() - 1);
        int col = solution.columns_used.at(random_idx);
        removed_cols.push_back(col);
        solution.remove_column(col, instance);
    }

    // Picks a random uncovered row and a random column that covers it
    int max_attempts = (int)solution.uncovered_rows.size() * 2;
    int failed_attempts = 0;

    while (!solution.uncovered_rows.empty()) {
        if (failed_attempts > max_attempts && !removed_cols.empty()) {  // Fallback
            solution.add_column(removed_cols.back(), instance);
            removed_cols.pop_back();
            failed_attempts = 0;
            continue;
        }

        // Picks a random uncovered row
        int row_idx = RandomGenerator::inRange(0, (int)solution.uncovered_rows.size() - 1);
        int row = solution.uncovered_rows.at(row_idx);

        std::vector<int> candidates;
        for (int col : instance.rows.at(row).covered_by)
            if (!util::is_in(solution.columns_used, col))
                candidates.push_back(col);

        if (candidates.empty()) {
            failed_attempts++;
            continue;
        }

        // Picks and adds a random candidate
        int chosen = candidates.at(RandomGenerator::inRange(0, (int)candidates.size() - 1));
        solution.add_column(chosen, instance);
        failed_attempts = 0;
    }
}
