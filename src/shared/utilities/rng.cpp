#include "shared/utilities/rng.hpp"

#include <random>
#include <utility>

double random(double min, double max) {
    static std::random_device rd;
    static std::mt19937 generator(rd());
    std::uniform_real_distribution<> distro(min, max);
    return distro(generator);
}

int randomInt(int min, int max) {
    static std::random_device rd;
    static std::mt19937 generator(rd());
    std::uniform_int_distribution<> distro(min, max);
    return distro(generator);
}