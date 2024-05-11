#include "server/game/player.hpp"
#include "shared/game/sharedobject.hpp"
#include "shared/game/stat.hpp"
#include <iostream>

SharedObject Player::toShared() {
    auto so = Creature::toShared();
    so.playerInfo = this->info;
    return so;
}

Player::Player(glm::vec3 corner, glm::vec3 facing):
    Creature(ObjectType::Player, corner, facing, ModelType::Player, SharedStats(
        Stat(0, 100, 100),
        Stat(0, 10, 5)
    ))
{
    this->info.is_alive = true;
    this->info.respawn_time = NULL;
}

Player::~Player() {

}