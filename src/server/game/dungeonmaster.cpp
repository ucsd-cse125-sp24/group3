#include "server/game/dungeonmaster.hpp"
#include "shared/game/sharedobject.hpp"
#include <iostream>

SharedObject DungeonMaster::toShared() {
    auto so = Object::toShared();
    return so;
}

DungeonMaster::DungeonMaster() : Object(ObjectType::DungeonMaster) {
    this->physics.movable = false; // not a movable player, just top-down view
}

DungeonMaster::~DungeonMaster() {

}