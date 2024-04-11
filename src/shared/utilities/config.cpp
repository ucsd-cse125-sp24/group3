#include "shared/utilities/config.hpp"

#include <nlohmann/json.hpp>
#include <iostream>
#include <fstream>

nlohmann::json parseConfig(int argc, char** argv) {
    if (argc > 2) {
        std::cerr << "Expected use:\n";
        std::cerr << "   (1) ./" << argv[0] << "\n";
        std::cerr << "   (2) ./" << argv[0] << " [path_to_config]\n";
        std::cerr << "Please pass either 0 or 1 command line arguments and try again."
            << std::endl;
        exit(1);
    }

    std::string filepath = "config.json";
    if (argc == 2) {
        filepath = argv[1];
    }

    try {
        std::fstream stream(filepath);
        return nlohmann::json::parse(stream);
    } catch (std::exception ex) {
        std::cerr << "Failed to parse config file, aborting.\n";
        std::cerr << "Most likely, you need specify the filepath to the config like so:\n";
        std::cerr << "    ./" << argv[0] << " [path_to_config]\n";
        std::cerr << "Alternatively, you can run the binaries from the directory\n";
        std::cerr << "in which the config file resides." << std::endl;
        exit(1);
    }
}