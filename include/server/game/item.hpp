#pragma once

#include "server/game/constants.hpp"
#include "server/game/object.hpp"
#include "shared/game/sharedobject.hpp"

class Item : public Object {
public:

	ItemInfo iteminfo;

	Item();
	~Item();

	virtual SharedObject toShared() override;
private:

};