#include "server/game/creature.hpp"
#include "shared/game/sharedobject.hpp"

SharedObject Creature::toShared() {
    auto so = Object::toShared();
    so.stats = this->stats;
    return so;
}