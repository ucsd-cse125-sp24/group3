#include "server/game/player.hpp"
#include "shared/game/sharedobject.hpp"
#include <iostream>

SharedObject Player::toShared() {
    auto so = Creature::toShared();
    return so;
}

Player::Player() : Creature(ObjectType::Player) {
}

Player::~Player() {

}