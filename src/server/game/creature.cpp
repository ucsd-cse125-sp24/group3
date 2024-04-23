#include "server/game/creature.hpp"
#include "shared/game/sharedobject.hpp"

SharedObject Creature::generateSharedObject() {
    auto so = Object::generateSharedObject();
    so.stats = this->stats;
}