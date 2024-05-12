#include "server/game/projectile.hpp"
#include "server/game/creature.hpp"
#include "server/game/servergamestate.hpp"

#include <iostream>

Projectile::Projectile(glm::vec3 corner, glm::vec3 facing, ModelType model, int damage):
    Object(ObjectType::Projectile, Physics(true, Collider::Box, corner, facing, glm::vec3(0.5f, 0.15f, 0.15f)), model)
{
    this->damage = damage;
    this->physics.velocityMultiplier = glm::vec3(2.0f, 0.25f, 2.0f);
    this->physics.velocity = glm::normalize(facing);
}

void Projectile::doCollision(Object* other, ServerGameState* state) {
    this->physics.velocity.x = 0;
    this->physics.velocity.z = 0;
    this->physics.collider = Collider::None;

    // do damage if creature
    Creature* creature = dynamic_cast<Creature*>(other);
    if (creature == nullptr) return;

    creature->stats.health.adjustBase(-this->damage);
}