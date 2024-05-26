#include "server/game/projectile.hpp"
#include "server/game/creature.hpp"
#include "server/game/servergamestate.hpp"
#include "server/game/spell.hpp"
#include "shared/audio/constants.hpp"

#include <iostream>

Projectile::Projectile(glm::vec3 corner, glm::vec3 facing, 
    glm::vec3 dimensions, ModelType model, std::optional<ServerSFX> destroy_sound, Options&& options):
    Object(ObjectType::Projectile, Physics(true, Collider::Box, corner, facing, dimensions), model),
    opt(options), destroy_sound(destroy_sound)
{
    this->physics.velocityMultiplier = glm::vec3(this->opt.h_mult, this->opt.v_mult, this->opt.h_mult);
    this->physics.velocity = glm::normalize(facing);
}

bool Projectile::doTick(ServerGameState& state) {
    if (this->physics.shared.corner.y == 0.0f) {
        state.markForDeletion(this->globalID);
    }

    if (!this->opt.homing) return false;
    this->opt.homing_duration--;
    if (this->opt.homing_duration <= 0) return false;

    Object* target = state.objects.getObject(*this->opt.target);
    if (target == nullptr) return false;

    auto pos_to_go_to = target->physics.shared.getCenterPosition();
    auto dir_to_target = glm::normalize(pos_to_go_to - this->physics.shared.getCenterPosition());

    this->physics.velocity += dir_to_target * this->opt.homing_strength;
    this->physics.velocity = glm::normalize(this->physics.velocity);
    std::cout << glm::to_string(this->physics.velocity) <<'\n';
    this->physics.shared.facing = this->physics.velocity;

    return true;
}

void Projectile::doCollision(Object* other, ServerGameState& state) {
    this->physics.velocity.x = 0;
    this->physics.velocity.z = 0;
    this->physics.collider = Collider::None;

    state.markForDeletion(this->globalID);
    if (this->destroy_sound.has_value()) {
        state.soundTable().addNewSoundSource(SoundSource(
            this->destroy_sound.value(),
            this->physics.shared.getCenterPosition(),
            DEFAULT_VOLUME,
            MEDIUM_DIST,
            MEDIUM_ATTEN
        ));
    }

    if (!this->opt.isSpell) {
        // do damage if creature
        Creature* creature = dynamic_cast<Creature*>(other);
        if (creature == nullptr) return;

        creature->stats.health.decrease(this->opt.damage);
    } 
    //handle cases for spell projectiles
    else {
        SpellOrb* orb = dynamic_cast<SpellOrb*>(this);
        switch (orb->sType) {
        case SpellType::Fireball: {
            // do damage if creature
            Creature* creature = dynamic_cast<Creature*>(other);
            if (creature != nullptr) {
                creature->stats.health.decrease(this->opt.damage);
                return;
            }

            // destory wall if it hits a wall
            // Implement "weak" wall that can break in few hits
            // before enabling this

            /*
            SolidSurface* wall = dynamic_cast<SolidSurface*>(other);
            if (wall != nullptr && wall->shared.surfaceType == SurfaceType::Wall) {
                state.markForDeletion(wall->globalID);
                return;
            } */
        }

        case SpellType::HealOrb: {
            // heal if creature
            Creature* creature = dynamic_cast<Creature*>(other);
            if (creature != nullptr) {
                creature->stats.health.increase(this->opt.damage);
                return;
            }
        }
        }
    }
}