#pragma once

#include <nlohmann/json.hpp>

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
nlohmann::json parseConfig(int argc, char** argv);