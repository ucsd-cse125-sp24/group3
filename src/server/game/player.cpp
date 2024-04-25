#include "server/game/player.hpp"
#include "shared/game/sharedobject.hpp"

SharedObject Player::toShared() {
    auto so = Creature::toShared();
    return so;
}