#include "data_structure.hpp"
#include "population.hpp"
#include <iostream>
#include <fstream>
#include <filesystem>

int main(int argc, char* argv[]) {
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

    std::filesystem::create_directory("results");
    std::filesystem::create_directory("results/convergence");

    for (const auto& file_path : instance_files) {
        SCPInstance instance;
        if (!instance.read_file(file_path)) {
            std::cerr << "Error reading " << file_path << "\n";
            continue;
        }

        std::vector<double> history;
        SCPSolution best = genetic_algorithm(instance, &history);

        // Extract clean instance name, e.g. "Testing/Wren_04.dat" -> "Wren_04"
        std::string clean_name = file_path;
        auto slash = clean_name.find_last_of('/');
        if (slash != std::string::npos) clean_name = clean_name.substr(slash + 1);
        auto dot = clean_name.find_last_of('.');
        if (dot != std::string::npos) clean_name = clean_name.substr(0, dot);

#ifndef NO_LOCAL_SEARCH
        std::string out_path = "results/convergence/" + clean_name + "_with_ls.csv";
#else
        std::string out_path = "results/convergence/" + clean_name + "_without_ls.csv";
#endif

        std::ofstream csv(out_path);
        csv << "generation,best_cost\n";
        for (size_t gen = 0; gen < history.size(); gen++) {
            csv << gen << "," << history[gen] << "\n";
        }
        csv.close();

        std::cout << file_path << " -> final cost = " << best.cost
                   << " (" << history.size() << " generations) saved to " << out_path << "\n";
    }

    return 0;
}