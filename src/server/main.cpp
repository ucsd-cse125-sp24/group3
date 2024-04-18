#include <iostream>

#include "server/server.hpp"
#include "shared/utilities/rng.hpp"

int main() {
    Server server; // cppcheck-suppress *

    std::cout << "I am a server!\n"; 

    // Test that shared lib is linked correctly
    std::cout << "Random number: " << randomInt(0, 100) << "\n";}
