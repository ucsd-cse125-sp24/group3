#include "server/game/player.hpp"
#include "shared/game/sharedobject.hpp"
#include "shared/game/stat.hpp"
#include "server/game/constants.hpp"
#include "server/game/item.hpp"
#include <iostream>

SharedObject Player::toShared() {
    auto so = Creature::toShared();
    so.playerInfo = this->info;
    so.inventoryInfo = this->sharedInventory;
    so.compass = this->compass;
    return so;
}

Player::Player(glm::vec3 corner, glm::vec3 facing):
    Creature(ObjectType::Player, corner, facing, ModelType::Player, SharedStats(
        Stat(0, 100, 100),
        Stat(0, 10, 5)
    )),
    sharedInventory(SharedInventory { .selected = 1, .inventory_size = INVENTORY_SIZE, 
        .inventory = std::vector<ModelType>(INVENTORY_SIZE, ModelType::Frame),
        .usesRemaining = std::vector<int>(INVENTORY_SIZE, 0), .hasOrb = false }),
    compass(SharedCompass { .angle = 0 })
{
    this->info.is_alive = true;
    this->info.respawn_time = NULL;
    this->info.render = true;

    // initialize inventory as empty
    this->inventory = std::vector<SpecificID>(INVENTORY_SIZE, -1);
}

Player::~Player() {

}

bool Player::canBeTargetted() const {
    // cannot be seen / targetted if the player is dead or invisible
    return this->info.is_alive && this->info.render;
}