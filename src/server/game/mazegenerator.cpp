#include "server/game/mazegenerator.hpp"

#include <string>
#include <filesystem>
#include <iostream>

MazeGenerator::MazeGenerator(std::string path) {
    for (const auto & entry : std::filesystem::directory_iterator(path)) {
        std::cout << entry.path() << std::endl;
    }
}


Grid MazeGenerator::generate() {

}
