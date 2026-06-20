#include <vector>
#include <string>

class SCPColumn {
    public:
        float cost;
        std::vector<int> covered_rows;
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
        std::vector<int> columns_range;
        std::vector<int> rows_range;

        bool read_file(const std::string& file_path);

        void print_columns() const;
        void print_rows() const;
};

class SCPSolution {
    public:
        float cost;
        std::vector<int> columns_used;
        std::vector<int> uncovered_rows;
        SCPSolution(float cost, std::vector<int> columns_used, std::vector<int> unconvered_rows);
        bool is_valid();
        void remove_column(int column);
        void add_column(int column_idx, SCPInstance& instance);
};

SCPSolution empty_solution(SCPInstance& instance);
