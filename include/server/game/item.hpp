#pragma once

#include "server/game/object.hpp"
#include "server/game/servergamestate.hpp"

/**
 * Class for any possible items that can go in the inventory
 */
class Item : public Object {
public:
	SharedItemInfo iteminfo;

    /**
     * @param type      Type of Object
     * @param movable   Movable factor for physics
     * @param corner    Corner position of the item
     * @param model     Model applied for the item
     * @param dimensions Dimensions of the item
     */
	Item(ObjectType type, bool movable, glm::vec3 corner, ModelType model, glm::vec3 dimensions);

    virtual void useItem(Object* other, ServerGameState& state);

    void dropItem(Object* other, ServerGameState& state, float dropDistance);

	void doCollision(Object* other, ServerGameState& state) override;

	SharedObject toShared() override;

private:

};