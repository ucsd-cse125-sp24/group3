#pragma once

#include "Collider.hpp"

class SphereCollider : public Collider {
public:
	// (x,y,z) center position
	glm::vec3 center;

	float radius;

	SphereCollider();
	SphereCollider(glm::vec3 center, float radius);
	~SphereCollider();

	bool detectCollision(Collider* otherCollider);
	bool resolveCollision(Collider* otherCollider);
	Shape getShape();

private:

};