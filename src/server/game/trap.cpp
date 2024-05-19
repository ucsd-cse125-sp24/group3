#include "server/game/trap.hpp"

Trap::Trap(ObjectType type, bool movable, glm::vec3 corner, Collider collider, ModelType model, glm::vec3 dimensions):
    Object(type, Physics(movable, collider, corner, glm::vec3(0.0f), dimensions), model),
    info(SharedTrapInfo {.triggered = false} )
{
    this->is_dm_trap = false;
    this->expiration = std::chrono::system_clock::now();
}

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

void Trap::setIsDMTrap(bool is_dm_trap) {
    this->is_dm_trap = is_dm_trap;
}

void Trap::setExpiration(std::chrono::time_point<std::chrono::system_clock> expiration) {
    this->expiration = expiration;
}

bool Trap::getIsDMTrap() {
    return this->is_dm_trap;
}

std::chrono::time_point<std::chrono::system_clock> Trap::getExpiration() {
    return this->expiration;
}