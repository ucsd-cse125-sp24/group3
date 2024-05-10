#include "server/game/spiketrap.hpp"
#include "server/game/servergamestate.hpp"
#include "shared/utilities/rng.hpp"
#include "server/game/objectmanager.hpp"
#include <chrono>

using namespace std::chrono_literals;

const std::chrono::seconds SpikeTrap::ACTIVE_TIME = 4s;
const std::chrono::seconds SpikeTrap::TIME_UNTIL_RESET = 10s;

SpikeTrap::SpikeTrap():
    Trap(ObjectType::SpikeTrap) 
{
    this->setModel(ModelType::Cube);
    this->physics.collider == Collider::Box;
    this->physics.movable = false;
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

        bool is_underneath = (
            center_pos.x >= this->physics.shared.corner.x &&
            center_pos.x <= this->physics.shared.corner.x + this->physics.shared.dimensions.x &&
            center_pos.z >= this->physics.shared.corner.z &&
            center_pos.z <= this->physics.shared.corner.z + this->physics.shared.dimensions.z
        );

        // Trigger the trap if the player is underneath, and only on a random roll (per tick)
        // This can give the player time to realize they are standing under a trap.
        if (is_underneath && randomInt(0, 100) != 0) {
            return true;
        }
    }

    return false;
}

void SpikeTrap::trigger() {
    Trap::trigger();

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

void SpikeTrap::reset() {
    this->physics.movable = false;
    this->physics.shared.corner.y += 0.1;

    if (this->physics.shared.corner.y >= this->reset_corner.y) {
        Trap::reset();
    }
}