#include "shared/utilities/config.hpp"

#include <boost/dll/runtime_symbol_info.hpp>

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

    // With the cmake setup we know the executables are three directories down from the root of the
    // repository, where the config file lives
    boost::filesystem::path root_path = boost::dll::program_location().parent_path().parent_path().parent_path();

    boost::filesystem::path filepath = root_path / "config.json";
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
            .game = {
                .timestep_length_ms = std::chrono::milliseconds(json.at("game").at("timestep_length_ms")),
                .maze = {
                    .directory = json.at("game").at("maze").at("directory"),
                    .maze_file = json.at("game").at("maze").at("maze_file")
                }
            },
            .network = {
                .server_ip = json.at("network").at("server_ip"),
                .server_port = json.at("network").at("server_port")
            },
            .server = {
                .lobby_name = json.at("server").at("lobby_name"),
                .lobby_broadcast = json.at("server").at("lobby_broadcast"),
                .max_players = json.at("server").at("max_players")
            },
            .client = {
                .default_name = json.at("client").at("default_name"),
                .lobby_discovery = json.at("client").at("lobby_discovery")
            }
        };
    } catch (nlohmann::json::exception& ex) {
        std::cerr << "Error parsing config file: " << ex.what() << std::endl;
        std::exit(1);
    }
}

GameConfig getDefaultConfig() {
    return GameConfig {
        .game = {
            .timestep_length_ms = std::chrono::milliseconds(30),
            .maze = {
                .directory = "maps",
                .maze_file = "default_maze.maze"
            }
        },
        .network = {
            .server_ip = "localhost",
            .server_port = 2355
        },
        .server = {
            .lobby_name = "My Test Lobby",
            .lobby_broadcast = false,
            .max_players = 1
        },
        .client = {
            .default_name = "Player",
            .lobby_discovery = false
        }
    };
}