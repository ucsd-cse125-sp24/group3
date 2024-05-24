#pragma once

#include <chrono>
#include "server/game/trap.hpp"
#include "server/game/servergamestate.hpp"
#include "server/game/object.hpp"
#include "server/game/servergamestate.hpp"

/**
 * Trap which shoots arrows on a timer in a specified direction
 * 
 * The arrows fly until they collide with something, at which point they disappear.
 * 
 * Originally I wanted arrows to "stick" into walls, but this would lead to a large amount of
 * arrows stuck to the wall, causing the game state to be larger and larger.
 * 
 * We can probably re-implement this after optimizing the newtorking code to only send
 * info about objects that have updated.
 */
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

    /// how long from initial activation until it can activate again
    const static inline std::chrono::seconds TIME_UNTIL_RESET = 4s;
    const static inline float SIGHTLINE_M = 15;

    bool shouldTrigger(ServerGameState& state) override;
    void trigger(ServerGameState& state) override;

    bool shouldReset(ServerGameState& state) override;
    void reset(ServerGameState& state) override;

private:
    /// The time at which the trap last shot
    std::chrono::time_point<std::chrono::system_clock> shoot_time;
    /// The direction towards which the trap is shooting
    Direction dir;
};