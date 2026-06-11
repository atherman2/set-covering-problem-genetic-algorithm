#include "data_structure.hpp"

using namespace std;

SCPColumn::SCPColumn(float cost, vector<int> rows) {
    this->cost = cost;
    rows_covered = rows;
}

SCPRow::SCPRow(vector<int> columns) {
    covered_by = columns;
}
