#include "server/game/slime.hpp"
#include "server/game/enemy.hpp"
#include "server/game/servergamestate.hpp"

#include <chrono>

Slime::Slime(glm::vec3 corner, glm::vec3 facing, int size):
    Enemy(corner, facing, ObjectType::Slime, ModelType::Cube, SharedStats(
        Stat(0, 30, 30),
        Stat(0, 10, 3)
    ))
{
    this->physics.velocityMultiplier.y = 0.5;
    this->physics.shared.dimensions = glm::vec3(size, size, size);
    this->last_jump_time = std::chrono::system_clock::now();
}

bool Slime::doBehavior(ServerGameState& state) {
    if (this->physics.shared.corner.y == 0) {
        // when it lands again reset its lateral velocity
        this->physics.velocity.x = 0;
        this->physics.velocity.z = 0;
    }

    auto now = std::chrono::system_clock::now();
    if (now - this->last_jump_time > 5s) {
        this->physics.velocity.y += JUMP_SPEED * 1.75;

        auto players = state.objects.getPlayers();
        auto player = players.get(0);

        this->physics.shared.facing = glm::normalize(
            player->physics.shared.getCenterPosition() - this->physics.shared.getCenterPosition()
        );

        this->physics.velocity.x = this->physics.shared.facing.x;
        this->physics.velocity.z = this->physics.shared.facing.z;

        this->last_jump_time = now;
    }
}

void Slime::doCollision(Object* other, ServerGameState& state) {

}