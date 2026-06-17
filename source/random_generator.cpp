#include "random_generator.hpp"

std::mt19937 RandomGenerator::generator(std::random_device{}());

int RandomGenerator::inRange(int min, int max) {
    std::uniform_int_distribution<int> distribution(min, max);
    return distribution(generator);
}

double RandomGenerator::inRange(double min, double max) {
    std::uniform_real_distribution<double> distribution(min, max);
    return distribution(generator);
}
