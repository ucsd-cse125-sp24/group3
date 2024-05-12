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
    Trap(ObjectType::ArrowTrap, false, corner, Collider::None, ModelType::Cube, dimensions) 
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

    // TODO remove this stuff when projectile just takes a model
    // and we don't need to size it, instead probably just pass through
    // a model and direction
    const float ARROW_WIDTH = 0.2f;    
    const float ARROW_LENGTH = 1.0f;    
    const float ARROW_HEIGHT = 0.2f;

    float arrow_x_dim;
    float arrow_z_dim;

    switch (this->dir) {
        case ArrowTrap::Direction::UP:
        case ArrowTrap::Direction::DOWN:
            arrow_x_dim = ARROW_WIDTH;
            arrow_z_dim = ARROW_LENGTH;
            break;
        case ArrowTrap::Direction::LEFT:
        case ArrowTrap::Direction::RIGHT:
            arrow_x_dim = ARROW_LENGTH;
            arrow_z_dim = ARROW_WIDTH;
            break;
    }

    state.objects.createObject(new Projectile(
        this->physics.shared.getCenterPosition(),
        this->physics.shared.facing, 
        glm::vec3(arrow_x_dim, 0.2f, arrow_z_dim),
        ModelType::Cube,
        Projectile::Options(10, 1.2f, 0.0f, false, false, 0.0f, {})
    ));

    this->shoot_time = std::chrono::system_clock::now();
}

bool ArrowTrap::shouldReset(ServerGameState& state) {
    auto now = std::chrono::system_clock::now();
    return (now - this->shoot_time > TIME_UNTIL_RESET);
}

void ArrowTrap::reset(ServerGameState& state) {
    Trap::reset(state);
}
