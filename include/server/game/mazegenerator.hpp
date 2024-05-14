#pragma once

#include <unordered_map>
#include <string>
#include <vector>
#include <boost/filesystem.hpp>
#include "server/game/grid.hpp"

enum class RoomType {
    EMPTY, // testing
    SPAWN,
    EASY,
    MEDIUM,
    HARD,
    LOOT,
    DIVERSE,
    EXIT
};

enum RoomEntry {
    T = 0b0001,
    B = 0b0010,
    L = 0b0100,
    R = 0b1000
};

enum class RoomSize {
    _10x10,
    _20x20,
    _40x40,
    INVALID
};

static std::unordered_map<RoomSize, std::vector<glm::ivec2>> TOP_ENTRY_COORDS = {
    { RoomSize::_10x10, {{4, 0}, {5, 0}} },
    { RoomSize::_20x20, {{4, 0}, {5, 0}, {14, 0}, {15, 0}} },
    { RoomSize::_40x40, {{4, 0}, {5, 0}, {14, 0}, {15, 0}, {24, 0}, {25, 0}, {34, 0}, {35, 0}} },
};
static std::unordered_map<RoomSize, std::vector<glm::ivec2>> LEFT_ENTRY_COORDS = {
    { RoomSize::_10x10, {{0, 4}, {0, 5}} },
    { RoomSize::_20x20, {{0, 4}, {0, 5}, {0, 14}, {0, 15}} },
    { RoomSize::_40x40, {{0, 4}, {0, 5}, {0, 14}, {0, 15}, {0, 24}, {0, 25}, {0, 34}, {0, 35}} },
};
static std::unordered_map<RoomSize, std::vector<glm::ivec2>> BOTTOM_ENTRY_COORDS = {
    { RoomSize::_10x10, {{4, 9}, {5, 9}} },
    { RoomSize::_20x20, {{4, 19}, {5, 19}, {14, 19}, {15, 19}} },
    { RoomSize::_40x40, {{4, 39}, {5, 39}, {14, 39}, {15, 39}, {24, 39}, {25, 39}, {34, 39}, {35, 39}} },
};
static std::unordered_map<RoomSize, std::vector<glm::ivec2>> RIGHT_ENTRY_COORDS = {
    { RoomSize::_10x10, {{9, 4}, {9, 5}} },
    { RoomSize::_20x20, {{19, 4}, {19, 5}, {19, 14}, {19, 15}} },
    { RoomSize::_40x40, {{39, 4}, {39, 5}, {39, 14}, {39, 15}, {39, 24}, {39, 25}, {39, 34}, {39, 35}} },
};

struct RoomClass {
    RoomType type;
    RoomSize size;
    uint8_t entries;
};

struct RoomClassHash { 
    size_t operator()(const RoomClass& p) const {
        size_t seed = 0;
        boost::hash_combine(seed, p.type);
        boost::hash_combine(seed, p.size);
        boost::hash_combine(seed, p.entries);
        return seed;
    }
};

class MazeGenerator {
public:
    MazeGenerator();

    Grid generate();

private:
    RoomType _getRoomType(boost::filesystem::path path);
    RoomSize _parseRoomSize(int rows, int columns);
    uint8_t _identifyEntryways(Grid& grid);
    void _validateRoom(Grid& grid, const RoomClass& rclass);

    void _loadRoom(boost::filesystem::path path);

    std::unordered_map<RoomClass, Grid, RoomClassHash> rooms;
};
