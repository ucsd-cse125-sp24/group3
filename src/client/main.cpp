#include <iostream>

#include "client/client.hpp"
#include "shared/utilities/rng.hpp"

int main() {
    Client client;

    std::cout << "I am a client!\n"; 

    // Test that shared lib is linked correctly
    std::cout << "Random number: " << randomInt(0, 100) << "\n"; // NOLINT
}
