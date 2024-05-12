#pragma once

#include <chrono>
#include "server/game/trap.hpp"
#include "server/game/servergamestate.hpp"
#include "server/game/object.hpp"
#include "server/game/servergamestate.hpp"

class FireballTrap: public Trap {
public:
    /**
     * @param corner Corner position of the fireball trap
     * @param dimensions dimensions of the fireball trap (probably will change once we use a non cube model to not have this)
     */
    FireballTrap(glm::vec3 corner, glm::vec3 dimensions);

    const static std::chrono::seconds TIME_UNTIL_RESET; // how long from initial activation until it can activate again
    const static int SHOOT_DIST; // how many grid cells away it can shoot you from

    bool shouldTrigger(ServerGameState& state) override;
    void trigger(ServerGameState& state) override;

    bool shouldReset(ServerGameState& state) override;
    void reset(ServerGameState& state) override;

    void doCollision(Object* other, ServerGameState* state) override;

private:
    std::chrono::time_point<std::chrono::system_clock> shoot_time;

    EntityID target;

    /**
     * TODO: optimize this to use spatial collision detection
     * @param object Object to test if this trap can see
     * @return distance in meters towards the object, or negative if cannot see.
     */
    float canSee(Object* object, ServerGameState* state);
};