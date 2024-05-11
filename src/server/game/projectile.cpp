#include "server/game/projectile.hpp"
#include "server/game/creature.hpp"

#include <iostream>

Projectile::Projectile(glm::vec3 corner, glm::vec3 facing, ModelType model, int damage):
    Object(ObjectType::Projectile, Physics(true, Collider::Box, corner, facing, glm::vec3(0.5f, 0.15f, 0.15f)), model)
{
    this->damage = damage;
    this->physics.velocityMultiplier = glm::vec3(3.0f, 0.5f, 3.0f);
    this->physics.velocity = facing;
}

void Projectile::doCollision(Object* other, ServerGameState* state) {
    this->physics.velocity = glm::vec3(0.0f, 0.0f, 0.0f);
    this->physics.movable = false;

    // do damage if creature
    Creature* creature = dynamic_cast<Creature*>(other);
    if (creature == nullptr) return;

    creature->stats.health.adjustBase(-this->damage);
}