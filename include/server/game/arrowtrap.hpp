#pragma once

#include <chrono>
#include "server/game/trap.hpp"
#include "server/game/servergamestate.hpp"
#include "server/game/object.hpp"
#include "server/game/servergamestate.hpp"

class ArrowTrap : public Trap {
public:
    /**
     * @param corner Corner position of the spike trap
     * @param dimensions dimensions of the spike trap (probably will change once we use a non cube model to not have this)
     */
    ArrowTrap(glm::vec3 corner, glm::vec3 dimensions);

    const static std::chrono::seconds TIME_UNTIL_RESET; // how long from initial activation until it can activate again

    bool shouldTrigger(ServerGameState& state) override;
    void trigger(ServerGameState& state) override;

    bool shouldReset(ServerGameState& state) override;
    void reset(ServerGameState& state) override;

    void doCollision(Object* other, ServerGameState* state) override;

private:
    std::chrono::time_point<std::chrono::system_clock> shoot_time;
};