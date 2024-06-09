#include "shared/utilities/config.hpp"

#include "shared/utilities/root_path.hpp"

#include <nlohmann/json.hpp>
#include <iostream>
#include <fstream>

GameConfig GameConfig::parse(int argc, char** argv) { // cppcheck-suppress constParameter
    if (argc > 2) {
        std::cerr << "Expected use:\n";
        std::cerr << "   (1) ./" << argv[0] << "\n";
        std::cerr << "   (2) ./" << argv[0] << " [path_to_config]\n";
        std::cerr << "Please pass either 0 or 1 command line arguments and try again."
            << std::endl;
        exit(1);
    }

    boost::filesystem::path filepath = "config.json";
    if (argc == 2) {
        filepath = argv[1];
    }

    nlohmann::json json;
    try {
        std::fstream stream(filepath.c_str());
        json = nlohmann::json::parse(stream);
    } catch (std::exception& ex) {
        std::cerr << "Failed to parse config file, aborting.\n";
        std::cerr << "Most likely, you need specify the filepath to the config like so:\n";
        std::cerr << "    " << argv[0] << " [path_to_config]\n";
        std::cerr << "Alternatively, you can verify that the executable is three directories below the config file,\n";
        std::cerr << "since those are the locations the program expects if you do not specify a config file path." << std::endl;
        std::exit(1);
    }

    try {
        return GameConfig {
            .server = {
                .port = json.at("server").at("port"),
                .lobby_name = json.at("server").at("lobby_name"),
                .lobby_broadcast = json.at("server").at("lobby_broadcast"),
                .max_players = json.at("server").at("max_players"),
                .disable_dm = json.at("server").at("disable_dm"),
                .skip_intro = json.at("server").at("skip_intro"),
                .maze = {
                    .directory = json.at("server").at("maze").at("directory"),
                    .procedural = json.at("server").at("maze").at("procedural"),
                    .maze_file = json.at("server").at("maze").at("maze_file")
                },
                .disable_enemies = json.at("server").at("disable_enemies") 
            },
            .client = {
                .lobby_discovery = json.at("client").at("lobby_discovery"),
                .fullscreen = json.at("client").at("fullscreen"),
                .fps_counter = json.at("client").at("fps_counter"),
                .presentation = json.at("client").at("presentation"),
                .render = json.at("client").at("render")
            }
        };
    } catch (nlohmann::json::exception& ex) {
        std::cerr << "Error parsing config file: " << ex.what() << std::endl;
        std::exit(1);
    }
}
