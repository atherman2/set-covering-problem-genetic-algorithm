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
    };

    const int RUNS_PER_INSTANCE = 10;

    std::filesystem::create_directory("results");
    std::ofstream csv("results/results.csv");
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
    std::cout << "\nResults saved in results.csv\n";
    return 0;
}