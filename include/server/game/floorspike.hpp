#pragma once

#include <chrono>
#include "server/game/trap.hpp"
#include "server/game/servergamestate.hpp"
#include "server/game/object.hpp"
#include "server/game/servergamestate.hpp"

class FloorSpike : public Trap {
public:
    enum class Orientation {
        Full, // take up whole grid cell
        Vertical, // 1/2 x, take up full z
        Horizontal // 1/2 z, take up full x
    };

    static const int DAMAGE;

    /**
     * @param corner Corner position of the floor spike trap
     * @param orientation what orientation the floorspike should be in
     * @param grid_width or how wide the longer axis should be (e.g. z if vertical, x if horizontal)
     */
    FloorSpike(glm::vec3 corner, Orientation dimensions, float grid_width);

    bool shouldTrigger(ServerGameState& state) override;

    bool shouldReset(ServerGameState& state) override;

    void doCollision(Object* other, ServerGameState* state) override;

private:
    std::chrono::time_point<std::chrono::system_clock> shoot_time;
};