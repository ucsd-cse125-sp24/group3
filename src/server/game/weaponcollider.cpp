#include "server/game/object.hpp"
#include "server/game/constants.hpp"
#include "server/game/creature.hpp"
#include "server/game/weaponcollider.hpp"
#include "server/game/servergamestate.hpp"
#include "shared/audio/constants.hpp"
#include <chrono>


WeaponCollider::WeaponCollider(Player* usedPlayer, glm::vec3 corner, glm::vec3 facing, glm::vec3 dimensions, ModelType model, WeaponOptions&& options):
    Object(ObjectType::WeaponCollider, Physics(true, Collider::None, corner, facing, dimensions), model),
    opt(options)
{
    this->physics.velocityMultiplier = glm::vec3(0.0f);
    this->preparing_time = std::chrono::system_clock::now();
    this->usedPlayer = usedPlayer;
    this->info.attacked = false;
    this->info.lightning = false;
    if(!this->opt.followPlayer) { 
        this->info.lightning = true; 
    }

    this->playSound = false;
    this->sound = ServerSFX::TEMP;
}

void WeaponCollider::doCollision(Object* other, ServerGameState& state) {
    Creature* creature = dynamic_cast<Creature*>(other);
    if (creature == nullptr) return;

    // don't dmg yourself
    if (this->usedPlayer != nullptr) {
        if (creature->globalID == this->usedPlayer->globalID) return;
    }
    
    // do damage if creature
    creature->stats.health.decrease(this->opt.damage);

    if (this->usedPlayer == nullptr) {
        auto knockback = glm::normalize(
            other->physics.shared.getCenterPosition() - this->physics.shared.getCenterPosition());

        creature->physics.currTickVelocity = 0.7f * knockback;
    }
    else {
        auto knockback = glm::normalize(
            other->physics.shared.getCenterPosition() - this->usedPlayer->physics.shared.getCenterPosition());

        creature->physics.currTickVelocity = 0.4f * knockback;
    }
}

void WeaponCollider::updateMovement(ServerGameState& state) {
    if(!this->opt.followPlayer) { return; }

    glm::vec3 attack_origin(
        this->usedPlayer->physics.shared.getCenterPosition().x,
        this->usedPlayer->physics.shared.getCenterPosition().y,
        this->usedPlayer->physics.shared.getCenterPosition().z
    );

    // Messy manual computation so it melees in the proper direction
    auto offset = 0.33f;
    auto multiplier = 2.2f;
    if (this->physics.shared.dimensions.x >= 2) {
        multiplier = 3.5f;
    }
    else if (this->physics.shared.dimensions.x > 1) {
        multiplier = 2.5f;
    }
    
    attack_origin += (this->usedPlayer->physics.shared.facing + glm::vec3(-offset)) * multiplier;
    attack_origin.y *= 0;

    state.objects.moveObject(this, attack_origin);
}

bool WeaponCollider::readyTime(ServerGameState& state) {
    if (this->info.attacked) { return true; }

    if (!this->playSound && this->info.lightning) {
        state.soundTable().addNewSoundSource(SoundSource(
            ServerSFX::Thunder,
            this->physics.shared.getCenterPosition(),
            DEFAULT_VOLUME,
            FAR_DIST,
            FAR_ATTEN
        ));
        this->playSound = true;
    }
    else if (!this->playSound) {
        state.soundTable().addNewSoundSource(SoundSource(
            this->sound,
            this->physics.shared.getCenterPosition(),
            MIDDLE_VOLUME,
            SHORT_DIST,
            SHORT_ATTEN
        ));
        this->playSound = true;
    }

    auto now = std::chrono::system_clock::now();
    std::chrono::duration<double> elapsed_milliseconds{ now - this->preparing_time };
    if (elapsed_milliseconds > std::chrono::milliseconds(this->opt.timeUntilAttack)) {
        this->info.attacked = true;
        this->attacked_time = now;
        this->physics.collider = Collider::Box;
        return true;
    }
    return false;
}

bool WeaponCollider::timeOut(ServerGameState& state) {
    auto now = std::chrono::system_clock::now();
    std::chrono::duration<double> elapsed_milliseconds{ now - this->attacked_time };
    if (elapsed_milliseconds > std::chrono::milliseconds(this->opt.attackDuration)) {
        return true;
    }
    return false;
}

SharedObject WeaponCollider::toShared() {
    auto so = Object::toShared();
    so.weaponInfo = this->info;
    return so;
}