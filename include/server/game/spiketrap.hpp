#pragma once

#include <chrono>
#include "server/game/trap.hpp"
#include "server/game/servergamestate.hpp"
#include "server/game/object.hpp"
#include "server/game/servergamestate.hpp"

class SpikeTrap : public Trap {
public:
    /**
     * @param corner Corner position of the spike trap
     * @param dimensions dimensions of the spike trap (probably will change once we use a non cube model to not have this)
     */
    SpikeTrap(glm::vec3 corner, glm::vec3 dimensions);

    const static int DAMAGE;
    const static std::chrono::seconds ACTIVE_TIME; // how long the spikes are down
    const static std::chrono::seconds TIME_UNTIL_RESET; // how long from initial activation until it can activate again

    bool shouldTrigger(ServerGameState& state) override;
    void trigger() override;

    bool shouldReset(ServerGameState& state) override;
    void reset() override;

    void doCollision(Object* other, ServerGameState* state) override;

private:
    std::chrono::time_point<std::chrono::system_clock> dropped_time;

    glm::vec3 reset_pos;
    glm::vec3 reset_corner;
    glm::vec3 reset_dimensions;
};