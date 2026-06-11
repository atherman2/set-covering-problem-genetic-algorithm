#include "data_structure.hpp"
#include <iostream>

int main(int argc, char* argv[]) {

    std::string file_path =
        (argc > 1) ? argv[1] : "Testing/Teste_01.dat";

    SCPInstance instance;

    if (!instance.read_file(file_path))
        return 1;

    std::cout << "Number of rows:    "
              << instance.num_rows << "\n";

    std::cout << "Number of columns: "
              << instance.num_columns << "\n\n";

    instance.print_columns();
    instance.print_rows();

    return 0;
}