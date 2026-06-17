#include <random>

class RandomGenerator {
public:
    RandomGenerator() = delete;

    static int inRange(int min, int max);
    static double inRange(double min, double max);
    static std::vector<int> uniqueRangeSet(int min, int max, int count);
    static std::vector<int> uniqueRangeShuffle(int min, int max, int count);

private:
    static std::mt19937 generator;
};
