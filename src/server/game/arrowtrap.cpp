#include "server/game/arrowtrap.hpp"
#include "server/game/servergamestate.hpp"
#include "shared/utilities/rng.hpp"
#include "server/game/objectmanager.hpp"
#include "server/game/projectile.hpp"
#include "server/game/collider.hpp"
#include <chrono>

using namespace std::chrono_literals;

const std::chrono::seconds ArrowTrap::TIME_UNTIL_RESET = 1s;
const int ArrowTrap::SHOOT_DIST = 5;

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

    glm::vec3 this_pos = this->physics.shared.getCenterPosition();

    auto players = state.objects.getPlayers();
    Player* player_to_shoot_at = nullptr;
    float closest_dist = std::numeric_limits<float>::max();
    for (int p = 0; p < players.size(); p++) {
        auto player = players.get(p);
        if (player == nullptr) continue;

        glm::vec3 player_pos = player->physics.shared.getCenterPosition();

        float curr_dist = this->canSee(player, &state);
        if (curr_dist < 0.0f) {
            continue;
        }
        if (closest_dist > curr_dist) {
            player_to_shoot_at = player;
            closest_dist = curr_dist;
        }
    }

    // convert grid units to actual distance values
    const float SHOOT_DIST_UNITS = Grid::grid_cell_width * ArrowTrap::SHOOT_DIST;
    if (closest_dist <= SHOOT_DIST_UNITS && player_to_shoot_at != nullptr) {
        this->physics.shared.facing = glm::normalize(player_to_shoot_at->physics.shared.getCenterPosition() - this_pos);

        return true;
    }

    return false;
}

void ArrowTrap::trigger(ServerGameState& state) {
    Trap::trigger(state);

    state.objects.createObject(new Projectile(
        this->physics.shared.getCenterPosition(), this->physics.shared.facing, ModelType::Cube, 10));

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

float ArrowTrap::canSee(Object* other, ServerGameState* state) {
    glm::vec3 other_pos = other->physics.shared.getCenterPosition();
    glm::vec3 this_pos = this->physics.shared.getCenterPosition();

    const int NUM_STEPS = 4;

    glm::vec3 step = (this_pos - other_pos) / static_cast<float>(NUM_STEPS);

    glm::vec3 curr_pos = this_pos;
    for (int i = 1; i < NUM_STEPS; i++) {
        curr_pos += step;
        auto walls = state->objects.getSolidSurfaces();
        // TODO: optimized collision detection
        for (int w = 0; w < walls.size(); w++) {
            auto wall = walls.get(w);
            if (wall == nullptr) continue;

            // TODO: actually use dimensions of the arrow
            // right now just assuming a small cube
            if (detectCollision(Physics(
                true, Collider::Box, curr_pos, glm::vec3(0.0f), glm::vec3(0.1f, 0.1f, 0.1f)),
                wall->physics)) 
            {
                return -1.0f;
            }
        }
    }

    float curr_dist = glm::distance(other_pos, this_pos);
    return curr_dist;
}
