#include "data_structure.hpp"
#include <fstream>
#include <sstream>
#include <iostream>
#include "util.hpp"

SCPColumn::SCPColumn(float cost, std::vector<int> rows, int num_rows)
	: cost(cost), covered_rows(std::move(rows)), covers_row(num_rows, false) {
	for (int row_idx : covered_rows)
		covers_row[row_idx] = true;
}

SCPRow::SCPRow(std::vector<int> columns)
	: covered_by(std::move(columns)) { }

bool SCPInstance::read_file(const std::string& file_path) {

    // Opening the file
    std::ifstream file(file_path);

    if (!file.is_open()) {
        std::cerr << "ERROR: could not open file: "
                  << file_path << std::endl;
        return false;
    }

    // Reads the tokens and ignores the text labels, keeping only the numbers
    std::string token;
    file >> token >> num_rows;      // discard "ROWS"
    file >> token >> num_columns;   // discard "COLUMNS"
    file >> token;                  // discard "DATA"

    columns.reserve(num_columns);
    rows.resize(num_rows, SCPRow({}));

    // Reading all columns...
    std::string line_text;
    std::getline(file, line_text); // discards the '\n' left after "DATA"

    for (int column_idx = 0; column_idx < num_columns; column_idx++) {
    	columns_range.push_back(column_idx);

        std::getline(file, line_text); // reads one full line
        std::stringstream line_stream(line_text);

        int column_id;
        double cost;

        line_stream >> column_id >> cost;

        // Reads all row indices covered by this column
        std::vector<int> covered_rows;
        int covered_row;

        while (line_stream >> covered_row) {
            int row_idx = covered_row - 1;  // file starts at 1, vector starts at 0

            covered_rows.push_back(row_idx);
            rows[row_idx].covered_by.push_back(column_idx); // row_idx is covered by column_idx
        }

        columns.emplace_back(cost, covered_rows, num_rows);
    }
    rows_range.resize(num_rows);
    for (int row_idx = 0; row_idx < num_rows; row_idx++)
    	rows_range.at(row_idx) = row_idx;
    return true;
}

void SCPInstance::print_columns() const {
    std::cout << "--- COLUMNS ---\n";

    for (int column_idx = 0; column_idx < num_columns; column_idx++) {
        std::cout << "Column " << column_idx + 1
                  << " | Cost: " << columns[column_idx].cost
                  << " | Covers rows: ";

        for (int row : columns[column_idx].covered_rows)
            std::cout << row + 1 << " ";

        std::cout << "\n";
    }
}

void SCPInstance::print_rows() const {
    std::cout << "\n--- ROWS ---\n";

    for (int row_idx = 0; row_idx < num_rows; row_idx++) {
        std::cout << "Row " << row_idx + 1
                  << " | Covered by columns: ";

        for (int column : rows[row_idx].covered_by)
            std::cout << column + 1 << " ";

        std::cout << "\n";
    }
}

SCPSolution::SCPSolution(float cost, std::vector<int> columns_used, std::vector<int> unconvered_rows, int num_columns)
	: cost(cost), columns_used(std::move(columns_used)), uncovered_rows(std::move(unconvered_rows)),
	  column_used(num_columns, false) {
	for (int col : this->columns_used)
		column_used[col] = true;
}

bool SCPSolution::is_valid() {
	return uncovered_rows.size() == 0;
}

void SCPSolution::remove_column(int column_idx, SCPInstance& instance) {
	util::remove(columns_used, column_idx);
	column_used[column_idx] = false;

	auto& column = instance.columns.at(column_idx);
	cost -= column.cost;

	for (int i = 0; i < column.covered_rows.size(); i++) {
		auto row_idx = column.covered_rows.at(i);
		auto& row = instance.rows.at(row_idx);

		bool still_covered = false;
		for (int j = 0; j < row.covered_by.size(); j++) {
			auto other_column_idx = row.covered_by.at(j);
			if (other_column_idx != column_idx && column_used[other_column_idx]) {
				still_covered = true;
				break;
			}
		}

		if (!still_covered)
			uncovered_rows.push_back(row_idx);
	}
}

void SCPSolution::add_column(int column_idx, SCPInstance& instance) {
	columns_used.push_back(column_idx);
	column_used[column_idx] = true;
	auto& column = instance.columns.at(column_idx);
	cost += column.cost;
	for (int i = 0; i < column.covered_rows.size(); i++) {
		auto row_idx = column.covered_rows.at(i);
		auto row_ref = util::search(uncovered_rows, row_idx);
		if (row_ref != uncovered_rows.end())
			util::remove_by_ref(uncovered_rows, *row_ref);
	}
}

void SCPSolution::print_columns_used() {
    std::sort(columns_used.begin(), columns_used.end());
    for (int i = 0; i < columns_used.size(); i++) {
        std::cout << (columns_used.at(i) + 1) << ", ";
    }
    std::cout << std::endl;
}

SCPSolution empty_solution(SCPInstance &instance) {
	return SCPSolution(0.0f, {}, instance.rows_range, instance.num_columns);
}
