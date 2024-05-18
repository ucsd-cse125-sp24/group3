#pragma once

#include "server/game/item.hpp"
#include "server/game/object.hpp"
#include <chrono>

class Orb : public Item {
public:
    /**
     * @param corner     Corner position of the Orb
     * @param dimensions Dimensions applied for the Orb
     */
    Orb(glm::vec3 corner, glm::vec3 dimensions);

    void useItem(Object* other, ServerGameState& state, int itemSelected) override;

private:
};