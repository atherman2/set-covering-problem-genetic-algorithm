#include <vector>

class SCPColumn {
    public:
        float cost;
        std::vector<int> rows_covered;
        SCPColumn(float cost, std::vector<int> rows);
};

class SCPRow {
    public:
        std::vector<int> covered_by;
        SCPRow(std::vector<int> columns);
};
