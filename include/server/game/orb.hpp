#pragma once

#include "server/game/item.hpp"
#include "server/game/object.hpp"
#include "server/game/servergamestate.hpp"
#include "server/game/item.hpp"
#include "shared/game/point_light.hpp"
#include <chrono>

class Orb : public Item {
public:
    /**
     * @param corner     Corner position of the Orb
     * @param dimensions Dimensions applied for the Orb
     * @param properties are point light properties of how the 
     * orb will light it's surroundings
     */
    Orb(glm::vec3 corner, glm::vec3 dimensions, const PointLightProperties& properties);

	void doCollision(Object* other, ServerGameState& state) override;
    void useItem(Object* other, ServerGameState& state, int itemSelected) override;

    void dropItem(Object* other, ServerGameState& state, int itemSelected, float dropDistance) override;

	SharedObject toShared() override;
private:
    PointLightProperties properties;
};