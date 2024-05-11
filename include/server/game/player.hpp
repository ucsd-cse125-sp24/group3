#pragma once

#include "server/game/constants.hpp"
#include "server/game/object.hpp"
#include "server/game/creature.hpp"
#include "shared/game/sharedobject.hpp"
#include "server/game/item.hpp"
#include <unordered_map>

class Player : public Creature {
public:
	SharedPlayerInfo info;
	SharedInventory sharedInventory;

	std::unordered_map<int, SpecificID> inventory;

	Player();
	~Player();

	virtual SharedObject toShared() override;

private:
};