#pragma once

#include <nlohmann/json.hpp>

#include <string>

/**
 * Struct which is a 1:1 mapping with the expected file format of the config.json
 * file, so that our code only has to reference this struct instead of having
 * to know the exact string indices to use to index into the nlohmann::json object.
 */
struct GameConfig {
    /// @brief Game config options
    struct {
        std::chrono::milliseconds timestep_length_ms;
    } game;
    /// @brief Shared config settings for the network
    struct {
        /// @brief IP that the server is being hosted on. E.g. "127.0.0.1" for localhost.
        std::string server_ip;
        /// @brief Port that the server is running on. This should be a value between
        /// 2302-2400, or 6073 so that it can be accepted through the firewall on lab computers
        int server_port;
    } network;
    /// @brief Config settings for the server
    struct {
        /// @brief Name of the server's lobby
        std::string lobby_name;
        /// @brief Whether or not the server should broadcast that the server is 
        bool lobby_broadcast;
        /// @brief max number of players this server allows
        int max_players;
    } server;
    /// @brief Config settings for the client
    struct {
        /// @brief Default name of the client
        std::string default_name;
        /// @brief Whether or not the client should listen for server lobby broadcasts
        bool lobby_discovery;
    } client;

    /**
     * Parses the config file and returns back a representation of the file
     * with all options specified. If the first command line argument
     * specifies a different config file, then it load that, but if no argument
     * is specified then it tries to look relative to the executable. It expects
     * the directory to look like
     * 
     * group3/
     *    - config.json
     *    - build/
     *       - bin/
     *           - client
     *           - server
     * 
     * So it looks relative from the binary location and assumes that the config.json
     * is at the root of the repository.
     * 
     * @param argc Number of command line arguments.
     * @param argv Command line arguments
     */
    static GameConfig parse(int argc, char** argv);
};
