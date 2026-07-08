#include "data_structure.hpp"
#include "population.hpp"
#include <iostream>
#include <fstream>
#include <chrono>
#include <filesystem>

int main(int argc, char* argv[]) {
    // List of instances to test
    std::vector<std::string> instance_files = {
        "Testing/Teste_01.dat",
        "Testing/Teste_02.dat",
        "Testing/Teste_03.dat",
        "Testing/Teste_04.dat",
        "Testing/Teste_05.dat",
        "Testing/Wren_01.dat",
        "Testing/Wren_02.dat",
        "Testing/Wren_03.dat",
        "Testing/Wren_04.dat",
    };

    const int RUNS_PER_INSTANCE = 10;

    std::filesystem::create_directory("results");
    #ifndef NO_LOCAL_SEARCH
        std::ofstream csv("results/results_with_ls.csv");
    #else
        std::ofstream csv("results/results_without_ls.csv");
    #endif
        csv << "instance,run,cost,time_seconds\n";

    for (const auto& file_path : instance_files) {
        SCPInstance instance;
        if (!instance.read_file(file_path)) {
            std::cerr << "Error reading " << file_path << "\n";
            continue;
        }

        std::cout << "Running instance: " << file_path << "\n";

        for (int run = 1; run <= RUNS_PER_INSTANCE; run++) {
            auto start = std::chrono::high_resolution_clock::now();

            SCPSolution best = genetic_algorithm(instance);

            auto end = std::chrono::high_resolution_clock::now();
            double elapsed = std::chrono::duration<double>(end - start).count();

            std::cout << "  Run " << run
                      << " -> cost = " << best.cost
                      << " | time = " << elapsed << "s\n";

            csv << file_path << "," << run << "," << best.cost << "," << elapsed << "\n";
        }
    }

    csv.close();
    #ifndef NO_LOCAL_SEARCH
        std::cout << "\nResults saved in results/results_with_ls.csv\n";
    #else
        std::cout << "\nResults saved in results/results_without_ls.csv\n";
    #endif
    return 0;
}