#include "server/game/player.hpp"
#include "shared/game/sharedobject.hpp"
#include "shared/game/stat.hpp"
#include "server/game/item.hpp"
#include "server/game/potion.hpp"
#include "server/game/servergamestate.hpp"
#include <iostream>

SharedObject Player::toShared() {
    auto so = Creature::toShared();
    return so;
}

Player::Player():
    Creature(ObjectType::Player, SharedStats(
        Stat(0, 100, 50),
        Stat(0, 10, 5)
    ))
{
    this->setModel(ModelType::Player);
}

Player::~Player() {

}