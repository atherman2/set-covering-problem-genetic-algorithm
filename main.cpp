#include "data_structure.hpp"
#include <iostream>

using namespace std;

int main() {
    auto column = SCPColumn(2.3, {2, 5, 8});
    cout << column.cost << " " << column.rows_covered.at(0) << endl;
    return 0;
}
