#pragma once

#include "server/game/constants.hpp"
#include "server/game/object.hpp"
#include "server/game/creature.hpp"
#include "shared/game/sharedobject.hpp"
#include <chrono>

class Weapon;

class DungeonMaster : public Creature {
public:
	SharedTrapInventory sharedTrapInventory;
	SharedDMInfo dmInfo;
	Weapon* lightning;

	DungeonMaster(glm::vec3 corner, glm::vec3 facing);
	~DungeonMaster();

	virtual SharedObject toShared() override;

	int getPlacedTraps();

	void setPlacedTraps(int placedTraps);

	// For lightning usage
	void useMana();
	void manaRegen();
		
private:
	int placedTraps;
	std::chrono::system_clock::time_point mana_used;
};