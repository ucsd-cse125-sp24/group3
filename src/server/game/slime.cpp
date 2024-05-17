#include "server/game/slime.hpp"
#include "server/game/enemy.hpp"
#include "server/game/servergamestate.hpp"
#include "shared/utilities/rng.hpp"

#include <chrono>

Slime::Slime(glm::vec3 corner, glm::vec3 facing, int size):
    Enemy(corner, facing, ObjectType::Slime, ModelType::Cube, SharedStats(
        Stat(0, 30, 30),
        Stat(0, 10, 3)
    ))
{
    this->size = size;
    this->physics.velocityMultiplier.y = 0.3;
    this->physics.velocityMultiplier.x = 0.3;
    this->physics.velocityMultiplier.z = 0.3;
    this->physics.shared.dimensions = glm::vec3(size, size, size);
    this->last_jump_time = std::chrono::system_clock::now();

    for (auto& time : this->JUMP_INTERVALS) {
        time += std::chrono::milliseconds(randomInt(-200, 200));
    }
    for (auto& str : this->JUMP_STRENGTHS) {
        str += 0.1f * randomInt(-1, 1);
    }

    this->jump_index = JUMP_INTERVALS.size() - 1;
}

bool Slime::doBehavior(ServerGameState& state) {
    if (this->physics.shared.corner.y == 0) {
        // when it lands again reset its lateral velocity
        this->physics.velocity.x = 0;
        this->physics.velocity.z = 0;
    }

    if (randomInt(1, 5) == 1) {
        this->stats.health.decrease(1);
    }

    auto now = std::chrono::system_clock::now();
    if (now - this->last_jump_time > JUMP_INTERVALS.at(this->jump_index)) {
        this->increaseJumpIndex();
        this->physics.velocity.y += JUMP_SPEED * 1.75;

        auto players = state.objects.getPlayers();
        auto player = players.get(0);

        this->physics.shared.facing = glm::normalize(
            player->physics.shared.getCenterPosition() - this->physics.shared.getCenterPosition()
        );

        this->physics.velocity.x = JUMP_STRENGTHS.at(this->jump_index) * this->physics.shared.facing.x;
        this->physics.velocity.z = JUMP_STRENGTHS.at(this->jump_index) * this->physics.shared.facing.z;

        this->last_jump_time = now;
    }
}

void Slime::doCollision(Object* other, ServerGameState& state) {
    Player* player = dynamic_cast<Player*>(other);
    if (player == nullptr) return;

    player->stats.health.decrease(1);
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
        state.objects.createObject(slime1);
        state.objects.createObject(slime2);
    }

    return true;
}

void Slime::increaseJumpIndex() {
    this->jump_index = (this->jump_index + 1) % this->JUMP_INTERVALS.size();
}
