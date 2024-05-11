#include "server/game/dungeonmaster.hpp"
#include "shared/game/sharedobject.hpp"
#include <iostream>

SharedObject DungeonMaster::toShared() {
    auto so = Object::toShared();
    return so;
}

DungeonMaster::DungeonMaster() : Object(ObjectType::DungeonMaster) {
    this->physics.feels_gravity = false;
}

DungeonMaster::~DungeonMaster() {

}