#pragma once

#include "server/game/constants.hpp"
#include "server/game/object.hpp"
#include "shared/game/sharedobject.hpp"
#include "shared/game/sharedgamestate.hpp"

class Item : public Object {
public:
	SharedItemInfo iteminfo;

	Item(ObjectType type);

	virtual void useItem();
	virtual void pickUpItem();

	virtual SharedObject toShared() override;

private:

};