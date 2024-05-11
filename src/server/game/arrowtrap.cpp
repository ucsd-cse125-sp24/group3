#include "server/game/arrowtrap.hpp"
#include "server/game/servergamestate.hpp"
#include "shared/utilities/rng.hpp"
#include "server/game/objectmanager.hpp"
#include "server/game/projectile.hpp"
#include <chrono>

using namespace std::chrono_literals;

const std::chrono::seconds ArrowTrap::TIME_UNTIL_RESET = 3s;

ArrowTrap::ArrowTrap(glm::vec3 corner, glm::vec3 dimensions):
    Trap(ObjectType::ArrowTrap, false, corner, Collider::None, ModelType::Cube, dimensions) 
{
    this->shoot_time = std::chrono::system_clock::now();
    this->physics.shared.facing = glm::vec3(1.0f, 0.0f, 0.0f);
}

bool ArrowTrap::shouldTrigger(ServerGameState& state) {
    if (this->info.triggered) {
        return false;
    }

    auto players = state.objects.getPlayers();
    for (int p = 0; p < players.size(); p++) {
        auto player = players.get(p);
        if (player == nullptr) continue;

        auto center_pos = player->physics.shared.getCenterPosition();

        bool is_in_front = (
            true
        );

        // Trigger the trap if the player is underneath, and only on a random roll (per tick)
        // This can give the player time to realize they are standing under a trap.
        if (is_in_front && randomInt(0, 100) != 0) {
            return true;
        }
    }

    return false;
}

void ArrowTrap::trigger(ServerGameState& state) {
    Trap::trigger(state);

    state.objects.createObject(new Projectile(
        this->physics.shared.corner, this->physics.shared.facing, ModelType::Cube, 10));

    this->shoot_time = std::chrono::system_clock::now();
}

bool ArrowTrap::shouldReset(ServerGameState& state) {
    auto now = std::chrono::system_clock::now();
    return (now - this->shoot_time > TIME_UNTIL_RESET);
}

void ArrowTrap::reset(ServerGameState& state) {
    Trap::reset(state);
}

void ArrowTrap::doCollision(Object* other, ServerGameState* state) {

}