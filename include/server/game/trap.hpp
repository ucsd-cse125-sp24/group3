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

    virtual bool shouldTrigger(ServerGameState& state) = 0;

    virtual void trigger(ServerGameState& state);

    virtual bool shouldReset(ServerGameState& state) = 0;

    virtual void reset(ServerGameState& state);

    SharedObject toShared() override;

protected:
    SharedTrapInfo info;
};