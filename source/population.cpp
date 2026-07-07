#include "population.hpp"
#include "constants.hpp"
#include "util.hpp"
#include "random_generator.hpp"
#include <algorithm>
#include <iostream>
#include <ostream>
#include <vector>

#include <queue>

SCPSolution fill_initial_population(std::vector<SCPSolution> &population, SCPInstance &instance) {
    auto sol = random_solution(instance);
    population.push_back(sol);
    auto best_sol = sol;

    for (int i = 2; i <= POPULATION_SIZE; i++) {
        sol = random_solution(instance);
        population.push_back(sol);
        if (sol.cost < best_sol.cost)
            best_sol = sol;
    }
    return best_sol;
}

SCPSolution random_solution(SCPInstance& instance) {
    SCPSolution sol = empty_solution(instance);
    auto unused_columns = instance.columns_range;

    std::vector<int> col_uncovered_count(instance.num_columns, 0);
    for(int c = 0; c < instance.num_columns; ++c) {
        col_uncovered_count[c] = instance.columns[c].covered_rows.size();
    }

    std::vector<bool> row_is_covered(instance.num_rows, false);

    int i = 1;
    while (sol.uncovered_rows.size() > 0) {
        i++;
        semi_greedy_add_column(unused_columns, sol, instance, col_uncovered_count, row_is_covered);
    }
    return sol;
}

void semi_greedy_add_column(std::vector<int>& unused_columns,
                            SCPSolution &solution,
                            SCPInstance &instance,
                            std::vector<int>& col_uncovered_count,
                            std::vector<bool>& row_is_covered) {

    std::priority_queue<columnEntry, std::vector<columnEntry>, std::greater<columnEntry>> min_heap;

    for (int col_idx : unused_columns) {
        int count = col_uncovered_count[col_idx];
        if (count == 0) continue;
        double score = static_cast<double>(count) / instance.columns[col_idx].cost;

        if (min_heap.size() < COL_RCL_SIZE) {
            min_heap.push({score, col_idx});
        } else if (score > min_heap.top().first) {
            min_heap.pop();
            min_heap.push({score, col_idx});
        }
    }

    std::vector<columnEntry> rcl;
    rcl.reserve(min_heap.size());
    while (!min_heap.empty()) {
        rcl.push_back(min_heap.top());
        min_heap.pop();
    }

    int random_idx = RandomGenerator::inRange(0, static_cast<int>(rcl.size()) - 1);
    int chosen_col = rcl[random_idx].second;

    solution.add_column(chosen_col, instance);

    for (int row_idx : instance.columns[chosen_col].covered_rows) {
        if (!row_is_covered[row_idx]) {
            row_is_covered[row_idx] = true;
            for (int col : instance.rows[row_idx].covered_by) {
                col_uncovered_count[col]--;
            }
        }
    }

    auto it = std::find(unused_columns.begin(), unused_columns.end(), chosen_col);
    if (it != unused_columns.end()) {
        std::swap(*it, unused_columns.back());
        unused_columns.pop_back();
    }
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
    for (int i = 0; (2 * i + 1) < parents.size(); i++) {
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
	auto selected_children_indexes = RandomGenerator::uniqueRangeSet(0, children.size() - 1, mutation_count);
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

void elitism(std::vector<SCPSolution>& population, std::vector<SCPSolution>& children) {
	std::vector<int> elite_indexes;
	int elite_size = (int) (population.size() * ELITISM_RATE);
	int i = 0;
	while (elite_indexes.size() < elite_size) {
		elite_indexes.push_back(i);
		i++;
	}
	while (i < population.size()) {
		auto worst_index = worst_in_vector(elite_indexes, population);
		if (population.at(i).cost < population.at(worst_index).cost) {
			util::remove(elite_indexes, worst_index);
			elite_indexes.push_back(i);
		}
		i++;
	}
	i = 0;
	int j = 0;
	while (i < children.size()) {
		if (!util::is_in(elite_indexes, j)) {
			population.at(j) = children.at(i);
			i++;
		}
		j++;
	}
}

int worst_in_vector(std::vector<int>& indexes, std::vector<SCPSolution>& population) {
	auto worst = indexes.at(0);
	for (int i = 0; i < indexes.size(); i++) {
		if (population.at(indexes.at(i)).cost > population.at(worst).cost)
			worst = indexes.at(i);
	}
	return worst;
}

SCPSolution apply_local_search(std::vector<SCPSolution>& solutions, SCPInstance& instance) {
    swap_local_search(solutions.at(0), instance);
    auto best_sol = solutions.at(0);
    for (int i = 1; i < solutions.size(); i++) {
        swap_local_search(solutions.at(i), instance);
        best_sol = best_solution(best_sol, solutions.at(i));
    }
    return best_sol;
}

SCPSolution genetic_algorithm(SCPInstance& instance) {
    int current_generation = 1;
    std::vector<SCPSolution> population, parents, children;
    SCPSolution best_known_solution = fill_initial_population(population, instance);
    auto best_first_gen = apply_local_search(population, instance);
    best_known_solution = best_solution(best_known_solution, best_first_gen);
    while (current_generation <= MAX_GENERATION) {
        std::cout << current_generation << "%," << std::endl;
    	parents = children = {};
        select_parents(parents, population);
        auto best_child = make_children(children, parents, instance);
        best_known_solution = best_solution(best_known_solution, best_child);
        auto best_mutated_child = mutate_children(children, instance);
        best_known_solution = best_solution(best_known_solution, best_mutated_child);
        auto best_new_gen = apply_local_search(children, instance);
        best_known_solution = best_solution(best_known_solution, best_new_gen);
        elitism(population, children);
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
