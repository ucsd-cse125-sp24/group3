#pragma once

#include "server/game/constants.hpp"
#include "server/game/object.hpp"
#include "server/game/creature.hpp"
#include "shared/game/sharedobject.hpp"


class DungeonMaster : public Creature {
public:
	SharedInventory sharedInventory;

	std::vector<SpecificID> inventory;

	DungeonMaster(glm::vec3 corner, glm::vec3 facing);
	~DungeonMaster();

	virtual SharedObject toShared() override;

	int getPlacedTraps();

	void setPlacedTraps(int placedTraps);
private:
	int placedTraps;
};