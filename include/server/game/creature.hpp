#pragma once

#include "server/game/constants.hpp"
#include "server/game/object.hpp"
#include "shared/game/sharedobject.hpp"

class Creature : public Object {
public:
	SharedStats stats;

	Creature(ObjectType type, 
		glm::vec3 corner,
		glm::vec3 facing,
		ModelType modelType,
		SharedStats stats);

	virtual ~Creature();

	virtual SharedObject toShared() override;
};