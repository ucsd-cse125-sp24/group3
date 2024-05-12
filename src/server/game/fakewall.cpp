#include "server/game/fakewall.hpp"
#include <chrono>

using namespace std::chrono_literals;

const std::chrono::seconds FakeWall::TIME_INVISIBLE = 2s;
const std::chrono::seconds FakeWall::TIME_VISIBLE = 6s;

FakeWall::FakeWall(glm::vec3 corner, glm::vec3 dimensions):
    Trap(ObjectType::FakeWall, false, corner, Collider::None, ModelType::Cube, dimensions)
{
    this->transition_time = std::chrono::system_clock::now();
}

bool FakeWall::shouldTrigger(ServerGameState& state) {
    auto now = std::chrono::system_clock::now();
    if (!this->info.triggered && now - this->transition_time > 0ms) {
        this->transition_time = now + TIME_VISIBLE;
        return true;
    }
    return false;
}

bool FakeWall::shouldReset(ServerGameState& state) {
    auto now = std::chrono::system_clock::now();
    if (this->info.triggered && now - this->transition_time > 0ms) {
        this->transition_time = now + TIME_INVISIBLE;
        return true;
    }
    return false;
}