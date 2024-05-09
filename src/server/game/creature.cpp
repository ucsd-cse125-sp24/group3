#include "server/game/creature.hpp"
#include "shared/game/sharedobject.hpp"
#include "shared/game/stat.hpp"

SharedObject Creature::toShared() {
    auto so = Object::toShared();
    so.stats = this->stats;
    return so;
}

Creature::Creature(ObjectType type, SharedStats stats): 
    Object(type), stats(stats)
{}

Creature::~Creature() {}