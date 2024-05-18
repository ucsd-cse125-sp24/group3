#include "server/game/dungeonmaster.hpp"
#include "shared/game/sharedobject.hpp"
#include <iostream>

SharedObject DungeonMaster::toShared() {
    auto so = Creature::toShared();
    return so;
}

DungeonMaster::DungeonMaster(glm::vec3 corner, glm::vec3 facing) : 
    Creature(ObjectType::DungeonMaster, corner, facing, ModelType::Cube, SharedStats(
    Stat(0, 100, 100),
    Stat(0, 10, 5)
)) {
    this->physics.feels_gravity = false;
    this->placedTraps = 0;
}

int DungeonMaster::getPlacedTraps() {
    return this->placedTraps;
}

void DungeonMaster::setPlacedTraps(int traps) {
    this->placedTraps = traps;
}

DungeonMaster::~DungeonMaster() {

}