#pragma once

#include "server/game/object.hpp"
#include "server/game/servergamestate.hpp"
#include "shared/game/sharedgamestate.hpp"
#include "server/game/servergamestate.hpp"

class Trap : public Object {
public:
    Trap(ObjectType type);

    virtual bool shouldTrigger(ServerGameState& state) = 0;

    virtual void trigger();

    virtual bool shouldReset(ServerGameState& state) = 0;

    virtual void reset();

    SharedObject toShared() override;

protected:
    SharedTrapInfo info;
};