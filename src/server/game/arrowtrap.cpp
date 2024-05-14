#include "server/game/arrowtrap.hpp"
#include "server/game/servergamestate.hpp"
#include "shared/utilities/rng.hpp"
#include "server/game/objectmanager.hpp"
#include "server/game/projectile.hpp"
#include "server/game/collider.hpp"
#include <chrono>

using namespace std::chrono_literals;

const std::chrono::seconds ArrowTrap::TIME_UNTIL_RESET = 4s;

ArrowTrap::ArrowTrap(glm::vec3 corner, glm::vec3 dimensions, ArrowTrap::Direction dir):
    Trap(ObjectType::ArrowTrap, false, corner, Collider::Box, ModelType::Cube, dimensions) 
{
    this->dir = dir;
    this->shoot_time = std::chrono::system_clock::now();
    switch (dir) {
        case ArrowTrap::Direction::LEFT:
            this->physics.shared.facing = glm::vec3(-1.0f, 0.0f, 0.0f);
            break;
        case ArrowTrap::Direction::RIGHT:
            this->physics.shared.facing = glm::vec3(1.0f, 0.0f, 0.0f);
            break;
        case ArrowTrap::Direction::UP:
            this->physics.shared.facing = glm::vec3(0.0f, 0.0f, -1.0f);
            break;
        case ArrowTrap::Direction::DOWN:
            this->physics.shared.facing = glm::vec3(0.0f, 0.0f, 1.0f);
            break;
    }
}

bool ArrowTrap::shouldTrigger(ServerGameState& state) {
    if (this->info.triggered) {
        return false;
    }
    return true;
}

void ArrowTrap::trigger(ServerGameState& state) {
    Trap::trigger(state);

    glm::vec3 arrow_origin(
        this->physics.shared.getCenterPosition().x,
        2.0f,
        this->physics.shared.getCenterPosition().z   
    );

    // TODO scale with grid size?
    switch (this->dir) {
        case ArrowTrap::Direction::UP:
            arrow_origin.z -= 3.0f;
            break;
        case ArrowTrap::Direction::DOWN:
            arrow_origin.z += 2.0f;
            break;
        case ArrowTrap::Direction::LEFT:
            arrow_origin.x -= 3.0f;
            break;
        case ArrowTrap::Direction::RIGHT:
            arrow_origin.x += 2.0f;
            break;
    }

    state.objects.createObject(new Arrow(arrow_origin,
        this->physics.shared.facing, this->dir));

    this->shoot_time = std::chrono::system_clock::now();
}

bool ArrowTrap::shouldReset(ServerGameState& state) {
    auto now = std::chrono::system_clock::now();
    return (now - this->shoot_time > TIME_UNTIL_RESET);
}

void ArrowTrap::reset(ServerGameState& state) {
    Trap::reset(state);
}
