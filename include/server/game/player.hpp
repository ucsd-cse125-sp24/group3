#pragma once

#include "server/game/constants.hpp"
#include "server/game/object.hpp"
#include "server/game/creature.hpp"
#include "shared/game/sharedobject.hpp"

class Player : public Creature {
public:
	SharedPlayerInfo info;

	Player(glm::vec3 corner, glm::vec3 facing);
	~Player();

	virtual SharedObject toShared() override;

private:
};