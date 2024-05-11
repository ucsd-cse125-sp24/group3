#include "server/game/creature.hpp"
#include "shared/game/sharedobject.hpp"
#include "shared/game/stat.hpp"

SharedObject Creature::toShared() {
    auto so = Object::toShared();
    so.stats = this->stats;
    return so;
}

Creature::Creature(ObjectType type,
    glm::vec3 corner,
    glm::vec3 facing,
    ModelType modelType,
    SharedStats&& stats):
    Object(type, Physics(true, Collider::Box, corner, facing), modelType), stats(stats)
    // dimensions in physics set by setModel in Object constructor
{}

Creature::~Creature() {}