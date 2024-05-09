#pragma once

#include "server/game/trap.hpp"
#include "server/game/servergamestate.hpp"
#include "server/game/object.hpp"

class SpikeTrap : public Trap {
public:
    SpikeTrap();

    bool shouldTrigger(const Object& state) override;
    void trigger() override;

private:
};