#pragma once

#include <string>
#include "server/game/grid.hpp"

class MazeGenerator {
public:
    MazeGenerator(std::string directory);

    Grid generate();    

private:
    std::vector<Grid> spawn_rooms;

    std::vector<Grid> easy_rooms;
    std::vector<Grid> medium_rooms;
    std::vector<Grid> hard_rooms;

    std::vector<Grid> exit_rooms;
};
