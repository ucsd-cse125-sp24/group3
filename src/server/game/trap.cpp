#include "server/game/trap.hpp"

Trap::Trap(ObjectType type):
    Object(type), info(SharedTrapInfo {.triggered = false} ) {}

void Trap::trigger() {
    this->info.triggered = true;
}

SharedObject Trap::toShared() {
    auto so = Object::toShared();
    so.trapInfo = this->info;
    return so;
}