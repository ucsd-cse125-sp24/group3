#include "server/game/dungeonmaster.hpp"
#include "shared/game/sharedobject.hpp"
#include "server/game/weapon.hpp"
#include <iostream>

SharedObject DungeonMaster::toShared() {
    auto so = Creature::toShared();
    so.trapInventoryInfo = this->sharedTrapInventory;
    return so;
}

DungeonMaster::DungeonMaster(glm::vec3 corner, glm::vec3 facing) : 
    Creature(ObjectType::DungeonMaster, corner, facing, ModelType::Cube, SharedStats(
    Stat(0, 100, 100),
    Stat(0, 10, 5)
)), sharedTrapInventory(SharedTrapInventory{ .selected = 1, .inventory_size = TRAP_INVENTORY_SIZE, .inventory = std::vector<ModelType>(TRAP_INVENTORY_SIZE, ModelType::Frame) }) {
    this->physics.feels_gravity = false;
    this->physics.velocityMultiplier = glm::vec3(3.0f, 1.0f, 3.0f);
    this->lightning = nullptr;

    this->placedTraps = 0;

    // TODO: fill in rest of traps
    this->sharedTrapInventory.inventory[0] = ModelType::FloorSpikeFull;
    this->sharedTrapInventory.inventory[1] = ModelType::FloorSpikeHorizontal;
    this->sharedTrapInventory.inventory[2] = ModelType::FloorSpikeVertical;
    this->sharedTrapInventory.inventory[3] = ModelType::SunGod;
    this->sharedTrapInventory.inventory[4] = ModelType::SpikeTrap;
    this->sharedTrapInventory.inventory[5] = ModelType::Lightning;
}

int DungeonMaster::getPlacedTraps() {
    return this->placedTraps;
}

void DungeonMaster::setPlacedTraps(int traps) {
    this->placedTraps = traps;
}

DungeonMaster::~DungeonMaster() {

}