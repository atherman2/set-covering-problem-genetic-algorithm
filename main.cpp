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

    //instance.print_columns();
    //instance.print_rows();

    SCPSolution sol = random_solution(instance);

    std::cout << "\n--- Solucao construida ---\n";
    std::cout << "Custo total: " << sol.cost << "\n";
    std::cout << "Numero de colunas usadas: " << sol.columns_used.size() << "\n";
    std::cout << "Linhas não cobertas: " << sol.uncovered_rows.size() << "\n";
    std::cout << "Solucao valida? " << (sol.is_valid() ? "SIM" : "NAO") << "\n";

    std::cout << "\n--- Teste de swap_local_search ---\n";
    swap_local_search(sol, instance);
    std::cout << "Custo depois: " << sol.cost << "\n";
    std::cout << "Colunas depois: " << sol.columns_used.size() << "\n";

    std::cout << "\n--- Teste de crossover ---\n";
    SCPSolution parent_a = random_solution(instance);
    SCPSolution parent_b = random_solution(instance);

    std::cout << "Custo pai A: " << parent_a.cost << "\n";
    std::cout << "Custo pai B: " << parent_b.cost << "\n";

    SCPSolution child = crossover(parent_a, parent_b, instance);

    std::cout << "Filho: custo = " << child.cost
              << ", colunas = " << child.columns_used.size()
              << ", valido = " << (child.is_valid() ? "SIM" : "NAO") << "\n";

    std::cout << "\n--- Teste de mutate ---\n";
    SCPSolution to_mutate = random_solution(instance);
    std::cout << "Custo antes: " << to_mutate.cost << "\n";
    std::cout << "Colunas antes: " << to_mutate.columns_used.size() << "\n";

    mutation(to_mutate, instance);

    std::cout << "Custo depois: " << to_mutate.cost << "\n";
    std::cout << "Colunas depois: " << to_mutate.columns_used.size() << "\n";
    std::cout << "Valido? " << (to_mutate.is_valid() ? "SIM" : "NAO") << "\n";
    
    return 0;
}