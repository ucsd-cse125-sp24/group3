#pragma once

#include "server/game/constants.hpp"
#include "server/game/object.hpp"
#include "server/game/creature.hpp"
#include "shared/game/sharedobject.hpp"
#include <unordered_map>

class Player : public Creature {
public:
	SharedPlayerInfo info;
	SharedInventory sharedInventory;

	std::unordered_map<int, SpecificID> inventory;

	/**
	 * @param Corner corner position of the player 
	 * @param facing what direction the player should spawn in facing
	 */
	Player(glm::vec3 corner, glm::vec3 facing);
	~Player();

	/**
	 * @brief This method returns true iff this Player has the Orb in their
	 * inventory
	 * @return true if this Player has the Orb in their inventory and false
	 * otherwise
	 */
	bool hasOrb();

	virtual SharedObject toShared() override;

private:
};