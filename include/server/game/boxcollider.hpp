#pragma once

#include "server/game/collider.hpp"

class BoxCollider : public Collider {
public:
	// min vector
	glm::vec3 min_position;

	// max vector
	glm::vec3 max_position;

	BoxCollider();
	BoxCollider(glm::vec3 minPos, glm::vec3 maxPos);
	~BoxCollider();

	bool detectCollision(Collider* otherBoundary) override;
	bool resolveCollision(Collider* otherBoundary) override;
	Shape getShape() override;

private:

};