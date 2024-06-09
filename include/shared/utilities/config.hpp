#pragma once

#include <nlohmann/json.hpp>

#include <string>

/**
 * Struct which is a 1:1 mapping with the expected file format of the config.json
 * file, so that our code only has to reference this struct instead of having
 * to know the exact string indices to use to index into the nlohmann::json object.
 */
struct GameConfig {
    /// @brief Config settings for the server
    struct {
        /// @brief port the server should run on
        int port;
        /// @brief Name of the server's lobby
        std::string lobby_name;
        /// @brief Whether or not the server should broadcast that the server is 
        bool lobby_broadcast;
        /// @brief max number of players this server allows
        int max_players;
        /// @brief whether or not the server will spawn a DM
        bool disable_dm;
        /// @brief whether or not to skip the intro cutscene
        bool skip_intro;
        struct {
            /**
             * @brief Path of the directory (contained in the repository
             * directory) that contains maze map files
             */
            std::string directory;

            /**
             * @brief whether or not to generate and use a procedurally generated maze
             */
            bool procedural;

            /**
             * @brief Name of the maze file within the maze maps directory that
             * the server should load.
             */
            std::string maze_file;

        } maze;
        /// @brief whether or not to disable enemy spawns
        bool disable_enemies;
    } server;
    /// @brief Config settings for the client
    struct {
        /// @brief Whether or not the client should listen for server lobby broadcasts
        bool lobby_discovery;
        /// @brief whether or not the client should open in fullscreen
        bool fullscreen;
        bool fps_counter;
        bool presentation;
        int render;
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
