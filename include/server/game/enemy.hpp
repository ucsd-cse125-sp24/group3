#pragma once

#include "server/game/constants.hpp"
#include "server/game/object.hpp"
#include "server/game/creature.hpp"
#include "shared/game/sharedobject.hpp"

class Enemy : public Creature {
public:
	/**
	 * @param Corner corner position of the enemy
	 * @param facing what direction the enemy should spawn in facing
	 */
	Enemy(glm::vec3 corner, glm::vec3 facing);
	~Enemy();

	virtual SharedObject toShared() override;
private:

};