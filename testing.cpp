#include "data_structure.hpp"
#include "population.hpp"
#include <iostream>
#include <fstream>
#include <chrono>
#include <filesystem>

int main(int argc, char* argv[]) {
    // Lista das instâncias a testar
    std::vector<std::string> instance_files = {
        "Testing/Teste_01.dat",
        // ... adicionar todas dps
    };

    const int RUNS_PER_INSTANCE = 10;

    std::filesystem::create_directory("results");
    std::ofstream csv("results/resultados.csv");
    csv << "instancia,execucao,custo,tempo_segundos\n";

    for (const auto& file_path : instance_files) {
        SCPInstance instance;
        if (!instance.read_file(file_path)) {
            std::cerr << "Erro ao ler " << file_path << "\n";
            continue;
        }

        std::cout << "Rodando instancia: " << file_path << "\n";

        for (int run = 1; run <= RUNS_PER_INSTANCE; run++) {
            auto start = std::chrono::high_resolution_clock::now();

            SCPSolution best = genetic_algorithm(instance);

            auto end = std::chrono::high_resolution_clock::now();
            double elapsed = std::chrono::duration<double>(end - start).count();

            std::cout << "  Execucao " << run
                      << " -> custo = " << best.cost
                      << " | tempo = " << elapsed << "s\n";

            csv << file_path << "," << run << "," << best.cost << "," << elapsed << "\n";
        }
    }

    csv.close();
    std::cout << "\nResultados salvos em resultados.csv\n";
    return 0;
}