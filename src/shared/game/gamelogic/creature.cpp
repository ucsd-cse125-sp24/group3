#include "shared/game/gamelogic/creature.hpp"
#include "shared/game/sharedgamestate/sharedobject.hpp"

SharedObject Creature::generateSharedObject() {
    auto so = Creature::generateSharedObject();
    so.stats = this->stats;
}