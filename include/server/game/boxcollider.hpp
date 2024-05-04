#pragma once

#include "server/game/collider.hpp"

class BoxCollider : public Collider {
public:
	// Vector for bottom left corner 
	glm::vec3 corner;

	// Vector for dimensions
	glm::vec3 dimension;

	BoxCollider();
	BoxCollider(glm::vec3 corner, glm::vec3 dimension);
	~BoxCollider();

	bool detectCollision(Collider* otherBoundary) override;
	bool resolveCollision(Collider* otherBoundary) override;
	Shape getShape() override;

private:

};