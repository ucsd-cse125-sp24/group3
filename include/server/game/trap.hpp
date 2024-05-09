#pragma once

#include "server/game/object.hpp"
#include "server/game/servergamestate.hpp"
#include "shared/game/sharedgamestate.hpp"

class Trap : public Object {
public:
    Trap(ObjectType type);

    virtual bool shouldTrigger(const Object& state) = 0;

    virtual void trigger();

    SharedObject toShared() override;

protected:
    SharedTrapInfo info;
};