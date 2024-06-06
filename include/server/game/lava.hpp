#pragma once

#include <chrono>
#include "server/game/trap.hpp"
#include "server/game/servergamestate.hpp"
#include "server/game/object.hpp"
#include "server/game/servergamestate.hpp"

class Lava : public Trap {
public:
    static const int DAMAGE;

    /**
     * @param corner Corner position of the floor spike trap
     * @param model_type type of model 
     * @param grid_width or how wide the longer axis should be (e.g. z if vertical, x if horizontal)
     */
    Lava(glm::vec3 corner, ModelType model_type, float grid_width);

    bool shouldTrigger(ServerGameState& state) override;

    bool shouldReset(ServerGameState& state) override;

    void doCollision(Object* other, ServerGameState& state) override;

private:
    std::chrono::time_point<std::chrono::system_clock> shoot_time;
};
