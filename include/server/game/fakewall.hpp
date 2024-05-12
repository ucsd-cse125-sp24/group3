#pragma once

#include <chrono>
#include "server/game/trap.hpp"
#include "server/game/servergamestate.hpp"
#include "server/game/object.hpp"
#include "server/game/servergamestate.hpp"

class FakeWall : public Trap {
public:
    /**
     * @param corner Corner position of the fake wall
     * @param dimensions dimensions of the fake wall
     */
    FakeWall(glm::vec3 corner, glm::vec3 dimensions);

    const static std::chrono::seconds TIME_VISIBLE; // how long it should stay visible for
    const static std::chrono::seconds TIME_INVISIBLE; // how long it should stay translucent for

    bool shouldTrigger(ServerGameState& state) override;
    bool shouldReset(ServerGameState& state) override;

    // we don't override trigger and reset because client side it will choose to render it depending
    // on whether it is triggered or not

private:
    std::chrono::time_point<std::chrono::system_clock> transition_time;
};