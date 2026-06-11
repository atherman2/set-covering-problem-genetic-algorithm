#include <vector>
#include <string>

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

class SCPInstance {
    public:
        int num_rows;
        int num_columns;

        std::vector<SCPColumn> columns;
        std::vector<SCPRow> rows;

        bool read_file(const std::string& file_path);

        void print_columns() const;
        void print_rows() const;
};