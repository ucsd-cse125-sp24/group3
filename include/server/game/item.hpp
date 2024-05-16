#pragma once

#include "server/game/object.hpp"
#include "server/game/servergamestate.hpp"

/*
* Things to do when adding items:
* 1. Make a class / or add onto existing inheritance tree
* 2. Add to gridcell.hpp / .cpp
* 3. Create the cell in servergamestate.cpp
* 4. Create a modeltype for item in sharedmodel.hpp
* 5. Make client.cpp render the item properly
* 6. Update img.hpp / .cpp & add the sprite in assets/img
* 7. Add this item in _sharedGameHUD, gui.cpp, and add text
*/

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