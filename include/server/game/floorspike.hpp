#pragma once

#include <chrono>
#include "server/game/trap.hpp"
#include "server/game/servergamestate.hpp"
#include "server/game/object.hpp"
#include "server/game/servergamestate.hpp"

/**
 * A spike trap which lines the floor and deals damage to players that walk over them
 */
class FloorSpike : public Trap {
public:
    static const int DAMAGE;

    /**
     * @param corner Corner position of the floor spike trap
     * @param orientation what orientation the floorspike should be in
     * @param grid_width or how wide the longer axis should be (e.g. z if vertical, x if horizontal)
     */
    FloorSpike(glm::vec3 corner, float grid_width);

    bool shouldTrigger(ServerGameState& state) override;

    bool shouldReset(ServerGameState& state) override;

    void doCollision(Object* other, ServerGameState& state) override;

private:
    std::chrono::time_point<std::chrono::system_clock> shoot_time;
};