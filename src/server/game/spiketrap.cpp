#include "server/game/spiketrap.hpp"
#include "server/game/servergamestate.hpp"
#include "shared/utilities/rng.hpp"
#include "server/game/objectmanager.hpp"
#include <chrono>

using namespace std::chrono_literals;

const int SpikeTrap::DAMAGE = 9999;
const std::chrono::seconds SpikeTrap::ACTIVE_TIME = 4s;
const std::chrono::seconds SpikeTrap::TIME_UNTIL_RESET = 10s;

SpikeTrap::SpikeTrap(glm::vec3 corner, glm::vec3 dimensions):
    Trap(ObjectType::SpikeTrap, false, corner, Collider::Box, ModelType::Cube, dimensions) 
{
    this->dropped_time = std::chrono::system_clock::now();
}

bool SpikeTrap::shouldTrigger(ServerGameState& state) {
    if (this->info.triggered) {
        return false;
    }

    auto now = std::chrono::system_clock::now();
    // only drop if it isn't currently triggered, and it has been at least 5 seconds since the 
    // last drop
    if (now - this->dropped_time < TIME_UNTIL_RESET) {
        return false;
    }

    auto players = state.objects.getPlayers();
    for (int p = 0; p < players.size(); p++) {
        auto player = players.get(p);
        if (player == nullptr) continue;

        auto center_pos = player->physics.shared.getCenterPosition();

        if (isUnderneath(player)) {
            return true;
        }
    }

    return false;
}

void SpikeTrap::trigger(ServerGameState& state) {
    Trap::trigger(state);

    this->reset_corner = this->physics.shared.corner;
    this->reset_dimensions = this->physics.shared.dimensions;

    this->physics.movable = true;
    this->physics.velocity.y = -8.0f * GRAVITY;

    this->dropped_time = std::chrono::system_clock::now();
}

bool SpikeTrap::shouldReset(ServerGameState& state) {
    auto now = std::chrono::system_clock::now();
    return (this->info.triggered && (now - this->dropped_time) > ACTIVE_TIME);
}

void SpikeTrap::reset(ServerGameState& state) {
    this->physics.movable = false;
    this->physics.shared.corner.y += 0.1;

    if (this->physics.shared.corner.y >= this->reset_corner.y) {
        Trap::reset(state);
    }
}

void SpikeTrap::doCollision(Object* other, ServerGameState* state) {
    auto creature = dynamic_cast<Creature*>(other);
    if (creature == nullptr) return; // not a creature, so don't really care

    // if it is falling
    if (this->physics.velocity.y < 0 && this->physics.shared.corner.y != 0) {
        creature->stats.health.adjustBase(-DAMAGE);
    }
}

bool SpikeTrap::isUnderneath(Object* other) {
    auto other_center = other->physics.shared.getCenterPosition();
    auto self_corner = this->physics.shared.corner;
    auto self_dims = this->physics.shared.dimensions;

    return (
        other_center.x >= self_corner.x &&
        other_center.x <= self_corner.x + self_dims.x &&
        other_center.z >= self_corner.z &&
        other_center.z <= self_corner.z + self_dims.z
    );
}