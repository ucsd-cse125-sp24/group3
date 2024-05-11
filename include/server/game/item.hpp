#pragma once

#include "server/game/object.hpp"
#include "shared/game/sharedobject.hpp"
#include "server/game/servergamestate.hpp"

class Item : public Object {
public:
	SharedItemInfo iteminfo;

	Item(ObjectType type);

	void doCollision(Object* other, ServerGameState* state) override;

	virtual SharedObject toShared() override;

private:

};