#pragma once

#include "server/game/constants.hpp"
#include "server/game/object.hpp"
#include "server/game/creature.hpp"
#include "shared/game/sharedobject.hpp"


class DungeonMaster : public Object {
public:
	DungeonMaster();
	~DungeonMaster();

	virtual SharedObject toShared() override;
private:

};