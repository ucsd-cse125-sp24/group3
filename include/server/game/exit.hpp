#pragma once

#include "server/game/object.hpp"
#include "shared/game/sharedobject.hpp"

class Exit : public Object {
public:
	SharedExit shared;

	Exit(bool open, glm::vec3 corner, glm::vec3 dimensions);

	void doCollision(Object* other, ServerGameState& state) override;

	virtual SharedObject toShared() override;
};