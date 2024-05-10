#pragma once

#include <chrono>
#include "server/game/trap.hpp"
#include "server/game/servergamestate.hpp"
#include "server/game/object.hpp"
#include "server/game/servergamestate.hpp"

class SpikeTrap : public Trap {
public:
    SpikeTrap();

    bool shouldTrigger(ServerGameState& state) override;
    void trigger() override;

    bool shouldReset(ServerGameState& state) override;
    void reset() override;

private:
    std::chrono::time_point<std::chrono::system_clock> dropped_time;

    glm::vec3 reset_pos;
    glm::vec3 reset_corner;
    glm::vec3 reset_dimensions;
};