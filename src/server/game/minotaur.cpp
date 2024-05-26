#include "server/game/minotaur.hpp"
#include "server/game/enemy.hpp"
#include "server/game/servergamestate.hpp"
#include "server/game/weapon.hpp"
#include "shared/audio/constants.hpp"
#include "shared/utilities/rng.hpp"

Minotaur::Minotaur(glm::vec3 corner, glm::vec3 facing) :
    Enemy(corner, facing, ObjectType::Minotaur, ModelType::Cube, SharedStats(
        Stat(0, 50, 50),
        Stat(0, 7, 3)
    ))
{
    this->last_charge_time = std::chrono::system_clock::now();
    this->chargeDelay = 8;
    this->chargeDuration = 3;

    this->physics.velocityMultiplier.y = 0;
    this->physics.velocityMultiplier.x = 0.3;
    this->physics.velocityMultiplier.z = 0.3;
    this->physics.shared.dimensions = glm::vec3(3.0f, 7.0f, 3.0f);
}

bool Minotaur::doBehavior(ServerGameState& state) {
    auto now = std::chrono::system_clock::now();
    std::chrono::duration<double> elapsed_seconds{ now - this->last_charge_time };

    if (elapsed_seconds > std::chrono::seconds(this->chargeDelay)) {
        auto players = state.objects.getPlayers();
        float closest_dist = std::numeric_limits<float>::max();
        Player* target = nullptr;
        for (int p = 0; p < players.size(); p++) {
            auto player = players.get(p);
            if (player == nullptr) continue;
            if (!player->canBeTargetted()) continue;

            float distance_to_player = glm::distance(this->physics.shared.corner, player->physics.shared.corner);
            if (distance_to_player < closest_dist) {
                closest_dist = distance_to_player;
                target = player;
            }
        }

        if (closest_dist < Grid::grid_cell_width * Minotaur::SIGHT_LIMIT_GRID_CELLS) {
            this->physics.shared.facing = glm::normalize(
                target->physics.shared.getCenterPosition() - this->physics.shared.getCenterPosition()
            );
        }
        else {
            this->physics.shared.facing = glm::normalize(glm::vec3(
                randomDouble(-1, 1), randomDouble(-1, 1), randomDouble(-1, 1)
            ));
        }

        this->physics.velocity.x = 1.5f * this->physics.shared.facing.x;
        this->physics.velocity.z = 1.5f * this->physics.shared.facing.z;
        this->last_charge_time = now;

        state.soundTable().addNewSoundSource(SoundSource(
            ServerSFX::Minotaur,
            this->physics.shared.getCenterPosition(),
            DEFAULT_VOLUME,
            MEDIUM_DIST,
            MEDIUM_ATTEN
        ));

        return true;
    } 
    else if (elapsed_seconds > std::chrono::seconds(this->chargeDuration)) {
        this->physics.velocity.x = 0;
        this->physics.velocity.z = 0;
        return true;
    }

    return false;
}

void Minotaur::doCollision(Object* other, ServerGameState& state) {
    Creature* creature = dynamic_cast<Creature*>(other);
    if (creature == nullptr) return;

    if (creature->type == ObjectType::Player) {
        creature->stats.health.decrease(3);
        creature->physics.velocity = 0.7f * glm::normalize(this->physics.shared.facing);
    }
}

bool Minotaur::doDeath(ServerGameState& state) {
    Enemy::doDeath(state);

    // Drop health potion upon death
    auto newCorner = this->physics.shared.corner;
    newCorner.y *= 0;
    if (randomInt(1, 4) == 4) {
        state.objects.createObject(new Weapon(newCorner, glm::vec3(1), WeaponType::Hammer));
    }
    return true;
}