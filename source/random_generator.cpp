#include "random_generator.hpp"
#include <unordered_set>
#include <algorithm>
#include <stdexcept>

std::mt19937 RandomGenerator::generator(std::random_device{}());

int RandomGenerator::inRange(int min, int max) {
    std::uniform_int_distribution<int> distribution(min, max);
    return distribution(generator);
}

double RandomGenerator::inRange(double min, double max) {
    std::uniform_real_distribution<double> distribution(min, max);
    return distribution(generator);
}

std::vector<int> RandomGenerator::uniqueRangeSet(int min, int max, int count) {
    if (count > (max - min + 1) || count < 0) {
        throw std::invalid_argument("Count exceeds range size or is negative.");
    }

    std::unordered_set<int> uniqueNumbers;
    std::uniform_int_distribution<int> distribution(min, max);

    while (uniqueNumbers.size() < static_cast<size_t>(count)) {
        uniqueNumbers.insert(distribution(generator));
    }

    return std::vector<int>(uniqueNumbers.begin(), uniqueNumbers.end());
}

std::vector<int> RandomGenerator::uniqueRangeShuffle(int min, int max, int count) {
    if (count > (max - min + 1) || count < 0) {
        throw std::invalid_argument("Count exceeds range size or is negative.");
    }

    std::vector<int> pool(max - min + 1);
    std::iota(pool.begin(), pool.end(), min);

    std::shuffle(pool.begin(), pool.end(), generator);

    if (count < static_cast<int>(pool.size())) {
        pool.resize(count);
    }

    return pool;
}
