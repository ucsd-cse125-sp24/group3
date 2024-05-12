#pragma once

#include <chrono>
#include "server/game/trap.hpp"
#include "server/game/servergamestate.hpp"
#include "server/game/object.hpp"
#include "server/game/servergamestate.hpp"

class ArrowTrap: public Trap {
public:
    enum class Direction {
        LEFT,
        UP,
        DOWN,
        RIGHT
    };

    /**
     * @param corner Corner position of the spike trap
     * @param dimensions TODO: remove once we use real model with size
     * @param dir What direction it should shoot in
     */
    ArrowTrap(glm::vec3 corner, glm::vec3 dimensions, Direction dir);

    const static std::chrono::seconds TIME_UNTIL_RESET; // how long from initial activation until it can activate again

    bool shouldTrigger(ServerGameState& state) override;
    void trigger(ServerGameState& state) override;

    bool shouldReset(ServerGameState& state) override;
    void reset(ServerGameState& state) override;

private:
    std::chrono::time_point<std::chrono::system_clock> shoot_time;
    Direction dir;
};