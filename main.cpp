#include "data_structure.hpp"
#include "population.hpp"
#include <iostream>
#include <chrono>

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

    auto start = std::chrono::high_resolution_clock::now();
    auto best_sol = genetic_algorithm(instance);
    auto end = std::chrono::high_resolution_clock::now();
    auto elapsed = std::chrono::duration<double>(end - start).count();
    std::cout << "Algorithm execution ended in " << elapsed << " seconds." << std::endl;
    std::cout << "Columns used by solution:" << std::endl;
    best_sol.print_columns_used();
    std::cout << std::endl << "Solution cost: " << best_sol.cost << std::endl;

    return 0;
}
