#include "server/game/slime.hpp"
#include "server/game/enemy.hpp"
#include "server/game/servergamestate.hpp"
#include "shared/utilities/rng.hpp"
#include "shared/game/status.hpp"
#include "shared/audio/constants.hpp"
#include "server/game/potion.hpp"

#include <chrono>

Slime::Slime(glm::vec3 corner, glm::vec3 facing, int size):
    Enemy(corner, facing, ObjectType::Slime, ModelType::Cube, SharedStats(
        Stat(0, 30, 30),
        Stat(0, 10, 3)
    ))
{
    this->jump_intervals = {3000ms, 2200ms, 2200ms};
    this->jump_strengths = {0.3f, 0.3f, 0.8f};

    this->size = size;
    this->physics.velocityMultiplier.y = 0.3;
    this->physics.velocityMultiplier.x = 0.3;
    this->physics.velocityMultiplier.z = 0.3;
    this->physics.shared.dimensions = glm::vec3(size, size, size);
    this->last_jump_time = std::chrono::system_clock::now();

    for (auto& time : this->jump_intervals) {
        time += std::chrono::milliseconds(randomInt(-200, 200)); // cppcheck-suppress useStlAlgorithm
    }
    for (auto& str : this->jump_strengths) {
        str += 0.1f * randomInt(-1, 1); // cppcheck-suppress useStlAlgorithm
    }

    this->jump_index = jump_intervals.size() - 1;
    this->landed = true;
}

bool Slime::doBehavior(ServerGameState& state) {
    bool mutated = false;
    
    if (this->physics.shared.corner.y == 0) {
        // only play land sound on first time this triggers per jump
        if (!this->landed) {
            this->landed = true;
            state.soundTable().addNewSoundSource(SoundSource(
                ServerSFX::SlimeLand,
                this->physics.shared.getCenterPosition(),
                DEFAULT_VOLUME,
                MEDIUM_DIST,
                MEDIUM_ATTEN
            ));
        }

        // when it lands again reset its lateral velocity
        this->physics.velocity.x = 0;
        this->physics.velocity.z = 0;
        mutated = true;
    }

    auto now = std::chrono::system_clock::now();
    if (now - this->last_jump_time > this->jump_intervals.at(this->jump_index)) {
        this->landed = false;
        state.soundTable().addNewSoundSource(SoundSource(
            ServerSFX::SlimeJump,
            this->physics.shared.getCenterPosition(),
            DEFAULT_VOLUME,
            MEDIUM_DIST,
            MEDIUM_ATTEN
        ));

        this->increaseJumpIndex();
        this->physics.velocity.y += JUMP_SPEED * 1.75;

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

        if (closest_dist < Grid::grid_cell_width * Slime::SIGHT_LIMIT_GRID_CELLS) {
            this->physics.shared.facing = glm::normalize(
                target->physics.shared.getCenterPosition() - this->physics.shared.getCenterPosition()
            );
        } else {
            this->physics.shared.facing = glm::normalize(glm::vec3(
                randomDouble(-1, 1), randomDouble(-1, 1), randomDouble(-1, 1)
            ));
        }


        this->physics.velocity.x = this->jump_strengths.at(this->jump_index) * this->physics.shared.facing.x;
        this->physics.velocity.z = this->jump_strengths.at(this->jump_index) * this->physics.shared.facing.z;

        this->last_jump_time = now;
        this->animState = AnimState::JumpAnim;
        mutated = true;
    }

    return mutated;
}

void Slime::doCollision(Object* other, ServerGameState& state) {
    Creature* creature = dynamic_cast<Creature*>(other);
    if (creature == nullptr) return;

    if (creature->type == ObjectType::Player) {
        creature->stats.health.decrease(1);
    }

    if (creature->type != ObjectType::Slime) {
        // have to apply 2 to make it not instantly get ticked away
        creature->statuses.addStatus(Status::Slimed, 2);
    }
}

bool Slime::doDeath(ServerGameState& state) {
    if (this->size > 1) {
        auto slime1 = new Slime(this->physics.shared.corner, this->physics.shared.facing, this->size - 1);
        slime1->physics.velocity.y += JUMP_SPEED;
        auto slime2 = new Slime(this->physics.shared.corner, this->physics.shared.facing, this->size - 1);
        slime2->physics.velocity.y += JUMP_SPEED;

        if (this->physics.velocity.x != 0) {
            slime2->physics.velocity.x = -this->physics.velocity.x;
            slime2->physics.velocity.z = -this->physics.velocity.z;
        } else {
            slime2->physics.velocity.x += 0.5f;
            slime2->physics.velocity.z += 0.5f;
        }
        SpecificID id1 = state.objects.createObject(slime1);
        SpecificID id2 = state.objects.createObject(slime2);

        state.spawner->addEnemy(state, id1);
        state.spawner->addEnemy(state, id2);
    }

    Enemy::doDeath(state);

    // Drop health potion upon death
    auto newCorner = this->physics.shared.corner;
    newCorner.y *= 0;
    // size 4 slime = 15 kills -> every 2 slime kill, get a potion
    if (randomInt(1,30) == 30) {
        state.objects.createObject(new Potion(newCorner, glm::vec3(1), PotionType::Health));
    }

    return true;
}

void Slime::increaseJumpIndex() {
    this->jump_index = (this->jump_index + 1) % this->jump_intervals.size();
}
