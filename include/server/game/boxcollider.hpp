#pragma once

#include "server/game/collider.hpp"

class BoxCollider : public Collider {
public:

	BoxCollider();
	BoxCollider(glm::vec3 corner, glm::vec3 dimension);
	~BoxCollider();

	bool detectCollision(Collider* otherBoundary) override;
	bool resolveCollision(Collider* otherBoundary) override;
	Shape getShape() override;

private:

};