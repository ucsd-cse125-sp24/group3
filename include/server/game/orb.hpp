#pragma once

#include "server/game/object.hpp"
#include "server/game/servergamestate.hpp"
#include "server/game/item.hpp"

class Orb : public Item {
public:
	Orb(glm::vec3 corner, glm::vec3 dimensions);

	void doCollision(Object* other, ServerGameState& state) override;
};