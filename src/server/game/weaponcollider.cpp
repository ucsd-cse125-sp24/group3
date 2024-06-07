#include "server/game/object.hpp"
#include "server/game/constants.hpp"
#include "server/game/creature.hpp"
#include "server/game/weaponcollider.hpp"
#include "server/game/servergamestate.hpp"
#include "server/game/item.hpp"
#include "server/game/mirror.hpp"
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

        //  If this weapon collider is a lightning bolt and it collides with
    //  a Player whose currently using a Mirror, then don't do any damage
    //  to the player and destroy the player's mirror.
    //  Also, paralyze the DM for some amount of time.
    if (this->info.lightning && creature->type == ObjectType::Player) {
        std::cout << "Applying lightning damage!" << std::endl;
        Player* player = dynamic_cast<Player*>(creature);

        //  Return early if this player is currently invulnerable to lightning
        if (player->isInvulnerableToLightning()) {
            std::cout << "Player is invulnerable to lightning - applying no damage." << std::endl;
            return;
        }

        for (int i = 0; i < player->inventory.size(); i++) {
            if (player->inventory[i] == -1)
                continue;

            //  Get item
            Item* item = state.objects.getItem(player->inventory[i]);

            //  If the item is a mirror and is used, then apply special
            //  behavior
            if (item->type == ObjectType::Mirror && item->iteminfo.used) {
                std::cout << "Player using a mirror got hit by a lightning bolt!" << std::endl;
                std::cout << "Deleting mirror!" << std::endl;

                Mirror* mirror = dynamic_cast<Mirror*>(item);

                //  Add mirror shatter sound effect
                state.soundTable().addNewSoundSource(SoundSource(
                    ServerSFX::MirrorShatter,
                    player->physics.shared.corner,
                    FULL_VOLUME,
                    FAR_DIST,
                    FAR_ATTEN
                ));

                //  Mark player as invulnerable to lightning for 1 second
                player->setInvulnerableToLightning(true, 1);

                //  Inform player they successfully relfected a lightning bolt
                //  using a mirror
                player->info.used_mirror_to_reflect_lightning = true;

                //  Destroy the mirror that the player is holding
                mirror->dropItem(player, state, i, 0.0f);
                state.markForDeletion(item->globalID);

                //  Remove mirror from player's list of used items
                player->sharedInventory.usedItems.erase(item->typeID);

                //  Paralyze the DM for 5 seconds
                state.objects.getDM()->setParalysis(true, 5);

                //  Don't apply damage to the player
                return;
            }
        }
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
            ServerSFX::ElectricHum,
            this->physics.shared.getCenterPosition(),
            DEFAULT_VOLUME,
            MEDIUM_DIST,
            MEDIUM_ATTEN 
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
        if (this->info.lightning) {
            state.soundTable().addNewSoundSource(SoundSource(
                ServerSFX::Thunder,
                this->physics.shared.getCenterPosition(),
                DEFAULT_VOLUME,
                FAR_DIST,
                FAR_ATTEN
            ));
        }
        this->playSound = true;
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