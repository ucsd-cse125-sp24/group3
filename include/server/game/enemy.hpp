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
	 * @param type Type of Enemy
	 * @param model model to render
	 * @param stats Starting stats
	 */
	Enemy(glm::vec3 corner, glm::vec3 facing, ObjectType type, ModelType model, SharedStats&& stats);
	~Enemy();

	/**
	 * @return true if the enemy was mutated, false otherwise
	 */
	virtual bool doBehavior(ServerGameState& state) {return false;};

	/**
	 * @return true if the enemy should be deleted, false otherwise
	 */
	virtual bool doDeath(ServerGameState& state);

	virtual SharedObject toShared() override;
private:

};