#pragma once

#include <chrono>
#include "server/game/trap.hpp"
#include "server/game/servergamestate.hpp"
#include "server/game/object.hpp"
#include "server/game/servergamestate.hpp"

/**
 * Spike trap which falls from the ceiling if a player walks underneath
 */
class TeleporterTrap : public Trap {
public:
    /**
     * @param corner Corner position of the teleporter trap
     */
    explicit TeleporterTrap(glm::vec3 corner);

    bool shouldTrigger(ServerGameState& state) override;
    void trigger(ServerGameState& state) override;

    bool shouldReset(ServerGameState& state) override;
    void reset(ServerGameState& state) override;

    void doCollision(Object* other, ServerGameState& state) override;

private:
};