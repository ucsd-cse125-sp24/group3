#include "shared/utilities/rng.hpp"

#include <random>
#include <utility>

double random(double min, double max) {
    static std::random_device random_device;
    static std::mt19937 generator(random_device());
    std::uniform_real_distribution<> distro(min, max);
    return distro(generator);
}

int randomInt(int min, int max) {
    static std::random_device random_device;
    static std::mt19937 generator(random_device());
    std::uniform_int_distribution<> distro(min, max);
    return distro(generator);
}