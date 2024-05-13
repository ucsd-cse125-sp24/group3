#include "server/game/trap.hpp"

Trap::Trap(ObjectType type, bool movable, glm::vec3 corner, Collider collider, ModelType model, glm::vec3 dimensions):
    Object(type, Physics(movable, collider, corner, glm::vec3(0.0f), dimensions), model),
    info(SharedTrapInfo {.triggered = false} )
{}

void Trap::trigger(ServerGameState& state) {
    this->info.triggered = true;
}

SharedObject Trap::toShared() {
    auto so = Object::toShared();
    so.trapInfo = this->info;
    return so;
}

void Trap::reset(ServerGameState& state) {
    this->info.triggered = false;
}