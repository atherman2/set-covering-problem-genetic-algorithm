#include <random>

class RandomGenerator {
public:
    RandomGenerator() = delete;

    static int inRange(int min, int max);
    static double inRange(double min, double max);

private:
    static std::mt19937 generator;
};
