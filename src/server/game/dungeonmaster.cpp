#include "server/game/dungeonmaster.hpp"
#include "shared/game/sharedobject.hpp"
#include "server/game/weapon.hpp"
#include <iostream>

SharedObject DungeonMaster::toShared() {
    auto so = Creature::toShared();
    so.trapInventoryInfo = this->sharedTrapInventory;
    so.DMInfo = this->dmInfo;
    return so;
}

DungeonMaster::DungeonMaster(glm::vec3 corner, glm::vec3 facing) : 
    Creature(ObjectType::DungeonMaster, corner, facing, ModelType::Cube, SharedStats(
    Stat(0, 100, 100),
    Stat(0, 10, 5)
)), sharedTrapInventory(SharedTrapInventory{ .selected = 1, .inventory_size = TRAP_INVENTORY_SIZE, \
    .inventory = std::vector<ModelType>(TRAP_INVENTORY_SIZE, ModelType::Frame) }), \
    dmInfo(SharedDMInfo{ .paralyzed = false, .mana_remaining = 15  })
{
    this->physics.feels_gravity = false;
    this->physics.velocityMultiplier = glm::vec3(3.0f, 1.0f, 3.0f);
    this->lightning = nullptr;
    this->mana_used = std::chrono::system_clock::now();
    this->placedTraps = 0;

    // TODO: fill in rest of traps
    this->sharedTrapInventory.inventory[0] = ModelType::Lightning;
    this->sharedTrapInventory.inventory[1] = ModelType::LightCut;
    this->sharedTrapInventory.inventory[2] = ModelType::ArrowTrap;
    this->sharedTrapInventory.inventory[3] = ModelType::SunGod;
    this->sharedTrapInventory.inventory[4] = ModelType::SpikeTrap;
    this->sharedTrapInventory.inventory[5] = ModelType::FloorSpikeFull;
    this->sharedTrapInventory.inventory[6] = ModelType::TeleporterTrap;

    //  DungeonMaster paralysis (relevant when the DM is paralyzed by a Player
    //  reflecting a lightning bolt back at the DM using a Mirror)

    //  Initially, the DM isn't paralyzed

    //  This will be overwritten when the DM is actually paralyzed using
    //  setParalysis()
    this->paralysisDuration = -1;
    
    //  This will be overwritten when the DM is actually paralyzed using
    //  setParalysis()
    this->paralysis_start_time = std::chrono::system_clock::now();
}

int DungeonMaster::getPlacedTraps() {
    return this->placedTraps;
}

void DungeonMaster::setPlacedTraps(int traps) {
    this->placedTraps = traps;
}

void DungeonMaster::useMana(int mana) {
    if (this->dmInfo.mana_remaining == DM_MANA_TOTAL) {
        this->mana_used = std::chrono::system_clock::now();
    }
    this->dmInfo.mana_remaining -= mana;
}   

void DungeonMaster::manaRegen() {
    if (this->dmInfo.mana_remaining == DM_MANA_TOTAL) return;

    auto now = std::chrono::system_clock::now();
    std::chrono::duration<double> elapsed_seconds{ now - this->mana_used };

    // Manually set to 0.5 seconds regen
    if (elapsed_seconds > std::chrono::milliseconds(500)) {
        this->dmInfo.mana_remaining += DM_MANA_REGEN;
        this->mana_used = now;
    }
}

DungeonMaster::~DungeonMaster() {

}

void DungeonMaster::setParalysis(bool isParalyzed, double paralysis_duration) {
    if (isParalyzed) {
        //  DM is now paralyzed - set duration and mark start timestamp
        std::cout << "Paralyzing the DM!" << std::endl;
        this->paralysisDuration = paralysis_duration;
        this->paralysis_start_time = std::chrono::system_clock::now();
    }

    this->dmInfo.paralyzed = isParalyzed;
}

bool DungeonMaster::isParalyzed() const {
    return this->dmInfo.paralyzed;
}

double DungeonMaster::getParalysisDuration() const {
    return this->paralysisDuration;
}

std::chrono::time_point<std::chrono::system_clock> DungeonMaster::getParalysisStartTime() const {
    return this->paralysis_start_time;
}