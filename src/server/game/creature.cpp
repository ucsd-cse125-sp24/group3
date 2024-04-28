#include "server/game/creature.hpp"
#include "shared/game/sharedobject.hpp"

SharedObject Creature::toShared() {
    auto so = Object::toShared();
    return so;
}

Creature::Creature(ObjectType type) : Object(type) {}

Creature::~Creature() {}