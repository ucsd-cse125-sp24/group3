#pragma once

#include "shared/utilities/serializable.hpp"
#include "shared/game/constants.hpp"
#include <string>
#include <vector>
#include <array>

class GameState : public Serializable {
public:
    GameState();
    
    // update all entities
    void update();

    // move entities that move
    void applyMovement();

    std::string serialize() const override;

    std::array<Tag, MAX_ENTITIES> tags;
    std::array<Trap, MAX_TRAPS> traps;
    std::array<Player, NUM_PLAYERS> players;
    DungeonMaster dm;
    GameData gameData;

private:

};

//Entity
using Entity = std::uint32_t;

//Component
using Tag = std::uint32_t;
using State = Tag;

namespace ComponentTags {
    const Tag Stand = 0;
    const Tag Walk = 1;
    const Tag Sprint = 2;
    const Tag Jump = 3;
    const Tag UseItem = 4;
    const Tag PickItem = 5;
    const Tag DropItem = 6;
}

namespace DMActionType {
    const int Slow = 0;
    const int Action2 = 1;
    const int Action3 = 2;
}

namespace ItemType {
    const int HealthPotion = 0;
    const int Item2 = 1;
    const int Item3 = 2;
};

struct DungeonMaster {
    int mana;
    std::array<int, MAX_SPELLS> actions;
};

struct Player {
    float speed;
    State currentState;
    std::vector<float> position;
    std::array<int, MAX_ITEMS> items;
};

struct Wall {
    bool illusion;
    std::vector<float> position;
};

struct Trap {
    bool used;
    std::vector<float> position;
};

struct Maze {
    int mapNumber;
    std::array<Wall, MAX_WALLS> walls;
};

struct GameData {
    float time;
};