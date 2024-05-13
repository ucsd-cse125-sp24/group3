#pragma once

#include "server/game/object.hpp"
#include "server/game/servergamestate.hpp"
#include "shared/game/sharedgamestate.hpp"
#include "server/game/servergamestate.hpp"

class Trap : public Object {
public:
    /**
     * @param type What type of trap this is
     * @param movable Whether or not the trap should be initialized being affected by gravity/velocity
     * @param corner Corner position of the trap
     * @param collider collider type for the trap
     * @param model What model should be used to render the trap
     * @param dimensions Dimensions to use for the trap, if it won't be overridden by the model info
     */
    Trap(ObjectType type, bool movable, glm::vec3 corner, Collider collider, ModelType model, glm::vec3 dimensions);

    /**
     * Determines if the trap should be triggered
     * 
     * @param state GameState, in case you need that to determine if the trap should trigger
     * @returns True if the trap should trigger, false otherwise
     */
    virtual bool shouldTrigger(ServerGameState& state) = 0;

    /**
     * Activates the trap
     * 
     * At the highest level, this just sets the triggered variable to true.
     * Subclass versions of this will also probably want to call this function
     * to set the triggered variable, in addition to any other common flags we
     * add in later.
     * 
     * @param state State in case triggering the trap needs to affect the gamestate
     */
    virtual void trigger(ServerGameState& state);

    /**
     * Resets the trap
     * 
     * @param state GameState, in case you need that to determine if the trap should reset
     * @returns True if the trap should reset, false otherwise
     */
    virtual bool shouldReset(ServerGameState& state) = 0;

    /**
     * Logic to reset the trap
     * 
     * At the highest level, this version just sets the triggered flag to false.
     * Derivations of this should do additional resetting logic, and potentially
     * call this Base class version as well to reset the common logic for all traps
     * (currently just the triggered variable)
     *
     * @param state GameState in case the trap needs to use the state for that
     */
    virtual void reset(ServerGameState& state);

    SharedObject toShared() override;

protected:
    SharedTrapInfo info;
};