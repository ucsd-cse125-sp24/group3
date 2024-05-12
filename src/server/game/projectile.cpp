#include "server/game/projectile.hpp"
#include "server/game/creature.hpp"
#include "server/game/servergamestate.hpp"

#include <iostream>

Projectile::Projectile(glm::vec3 corner, glm::vec3 facing, glm::vec3 dimensions, ModelType model, Options options):
    Object(ObjectType::Projectile, Physics(true, Collider::Box, corner, facing, dimensions), model),
    opt(options)
{
    this->physics.velocityMultiplier = glm::vec3(this->opt.h_mult, this->opt.v_mult, this->opt.h_mult);
    this->physics.velocity = glm::normalize(facing);
}

void Projectile::doTick(ServerGameState* state) {
    if (!this->opt.homing) return;
    Object* target = state->objects.getObject(this->opt.target);
    if (target == nullptr) return;

    auto pos_to_go_to = target->physics.shared.getCenterPosition();
    auto dir_to_target = glm::normalize(pos_to_go_to - this->physics.shared.getCenterPosition());

    this->physics.velocity += dir_to_target * this->opt.homing_strength;
    this->physics.velocity = glm::normalize(this->physics.velocity);
    this->physics.shared.facing = this->physics.velocity;
}

void Projectile::doCollision(Object* other, ServerGameState* state) {
    this->physics.velocity.x = 0;
    this->physics.velocity.z = 0;
    this->physics.collider = Collider::None;

    if (this->opt.disappearOnContact) {
        state->markForDeletion(this->globalID);
    }

    // do damage if creature
    Creature* creature = dynamic_cast<Creature*>(other);
    if (creature == nullptr) return;

    creature->stats.health.adjustBase(-this->opt.damage);
}