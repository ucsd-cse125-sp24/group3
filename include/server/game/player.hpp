#pragma once

#include "server/game/constants.hpp"
#include "server/game/object.hpp"
#include "server/game/creature.hpp"
#include "shared/game/sharedobject.hpp"
#include <vector>

class Player : public Creature {
public:
	SharedPlayerInfo info;
	SharedInventory sharedInventory;

	std::vector<SpecificID> inventory;

	/**
	 * @param Corner corner position of the player 
	 * @param facing what direction the player should spawn in facing
	 */
	Player(glm::vec3 corner, glm::vec3 facing);
	~Player();

	virtual SharedObject toShared() override;

	bool canBeTargetted() const;

private:
};