#pragma once

#include <map>
#include <unordered_map>
#include <unordered_set>
#include <string>
#include <vector>
#include <queue>
#include <memory>
#include <optional>
#include <boost/filesystem.hpp>
#include "server/game/grid.hpp"
#define GLM_ENABLE_EXPERIMENTAL
#include "glm/gtx/hash.hpp"

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

#define ALL_TYPES { \
    RoomType::EMPTY, \
    RoomType::SPAWN, \
    RoomType::EASY, \
    RoomType::MEDIUM, \
    RoomType::HARD, \
    RoomType::LOOT, \
    RoomType::DIVERSE, \
    RoomType::EXIT, \
}

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

    bool operator==(const RoomClass& other) const {
        return this->type == other.type && this->size == other.size && this->entries == other.entries;
    }
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

struct Room {
    Room(Grid&& grid, const RoomClass& rclass, int id):
        grid(grid), rclass(rclass), id(id) {}

    Grid grid;
    RoomClass rclass;
    int id;
};

struct ivec2_comparator {
    bool operator()(const glm::ivec2& lhs, const glm::ivec2& rhs) const {
        if (lhs.x > rhs.x) {
            return false;
        }
        return (lhs.y < rhs.y);
    }
};

#define GRID_CELLS_PER_ROOM 10 // each room is 10x10 (or some multiple, but the unit level is 10)

class MazeGenerator {
public:
    MazeGenerator();

    Grid generate();

private:
    RoomType _getRoomType(boost::filesystem::path path);
    RoomSize _parseRoomSize(int rows, int columns);
    uint8_t _identifyEntryways(Grid& grid);
    void _validateRoom(Grid& grid, const RoomClass& rclass);

    std::vector<glm::ivec2> _getRoomCoordsTakenBy(RoomSize size, glm::ivec2 top_left);

    std::shared_ptr<Room> _pullRoomByType(RoomType type);
    std::shared_ptr<Room> _pullRoomByClass(const RoomClass& type);

    std::vector<glm::ivec2> _getAdjRoomCoords(std::shared_ptr<Room> room, glm::ivec2 origin_coord);

    // bool _hasOpenConnection(std::shared_ptr<Room> room, glm::ivec2 origin_coord);

    std::queue<glm::ivec2> frontier;
    void _placeRoom(std::shared_ptr<Room> room, glm::ivec2 origin_coord);

    bool _isOpenWorldCoord(glm::ivec2 coord);

    std::map<glm::ivec2, int, ivec2_comparator> maze;

    void _loadRoom(boost::filesystem::path path);
    std::unordered_map<RoomClass, std::shared_ptr<Room>, RoomClassHash> rooms_by_class;
    std::unordered_map<RoomType, std::vector<std::shared_ptr<Room>>> rooms_by_type;
    std::unordered_map<int , std::shared_ptr<Room>> rooms_by_id;

    int _next_room_id;
};
