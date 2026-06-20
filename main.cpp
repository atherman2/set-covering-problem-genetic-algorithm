#include "data_structure.hpp"
#include "population.hpp"
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

    SCPSolution sol = random_solution(instance);

    std::cout << "\n--- Solucao construida ---\n";
    std::cout << "Custo total: " << sol.cost << "\n";
    std::cout << "Numero de colunas usadas: " << sol.columns_used.size() << "\n";
    std::cout << "Linhas não cobertas: " << sol.uncovered_rows.size() << "\n";
    std::cout << "Solucao valida? " << (sol.is_valid() ? "SIM" : "NAO") << "\n";

    return 0;
}