#pragma once

#include "server/game/collider.hpp"

class SphereCollider : public Collider {
public:
	// Vector for (x,y,z) center position
	glm::vec3 center;

	float radius;

	SphereCollider();
	SphereCollider(glm::vec3 center, float radius);
	~SphereCollider();

	bool detectCollision(Collider* otherCollider) override;
	bool resolveCollision(Collider* otherCollider) override;
	Shape getShape() override;

private:

};