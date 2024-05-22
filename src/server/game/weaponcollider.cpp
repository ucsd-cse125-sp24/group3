#include "server/game/object.hpp"
#include "server/game/constants.hpp"
#include "server/game/creature.hpp"
#include "server/game/weaponcollider.hpp"
#include "server/game/servergamestate.hpp"
#include <chrono>


WeaponCollider::WeaponCollider(Player* usedPlayer, glm::vec3 corner, glm::vec3 facing, glm::vec3 dimensions, ModelType model, WeaponOptions&& options):
    Object(ObjectType::WeaponCollider, Physics(true, Collider::None, corner, facing, dimensions), model),
    opt(options)
{
    this->physics.velocityMultiplier = glm::vec3(0.0f);
    this->preparing_time = std::chrono::system_clock::now();
    this->usedPlayer = usedPlayer;
    this->attacked = false;
}

void WeaponCollider::doCollision(Object* other, ServerGameState& state) {
    
    Creature* creature = dynamic_cast<Creature*>(other);
    if (creature == nullptr) return;
    // since the collider may hit yourself if looking down, just ignore this case
    if (creature->globalID == this->usedPlayer->globalID) return;

    // do damage if creature
    creature->stats.health.decrease(this->opt.damage);
}

void WeaponCollider::updateMovement(ServerGameState& state) {

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

bool WeaponCollider::readyTime() {
    if (this->attacked) { return true; }
    auto now = std::chrono::system_clock::now();
    std::chrono::duration<double> elapsed_milliseconds{ now - this->preparing_time };
    if ((now - this->preparing_time) > std::chrono::milliseconds(this->opt.timeUntilAttack)) {
        this->attacked = true;
        this->attacked_time = now;
        this->physics.collider = Collider::Box;
        return true;
    }
    return false;
}

bool WeaponCollider::timeOut() {
    auto now = std::chrono::system_clock::now();
    std::chrono::duration<double> elapsed_milliseconds{ now - this->attacked_time};
    return (now - this->attacked_time) > std::chrono::milliseconds(this->opt.attackDuration);
}

void WeaponCollider::removeAttack(ServerGameState& state) {
    state.markForDeletion(this->globalID);
}
