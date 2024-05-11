#pragma once

#include "server/game/constants.hpp"
#include "server/game/object.hpp"
#include "shared/game/sharedobject.hpp"

class Creature : public Object {
public:
	SharedStats stats;

	/**
	 * @param type Type of Object
	 * @param corner Corner position of object
	 * @param facing What direction the object is facing
	 * @param modelType what type of model to render for the creature
	 * @param stats Stats information for the creature (e.g. health, speed)
	 */
	Creature(ObjectType type, 
		glm::vec3 corner,
		glm::vec3 facing,
		ModelType modelType,
		SharedStats&& stats);

	virtual ~Creature();

	virtual SharedObject toShared() override;
};