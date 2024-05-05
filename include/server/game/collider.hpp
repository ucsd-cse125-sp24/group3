#pragma once

#include "server/game/constants.hpp"

class Collider {
public:
	// Vector for bottom left corner 
	glm::vec3 corner;

	// Vector for dimensions
	glm::vec3 dimension;

	enum Shape {   
		Box, 
		Sphere
	};

	virtual bool detectCollision(Collider* otherCollider) = 0;
	virtual bool resolveCollision(Collider* otherCollider) = 0;
	virtual Shape getShape() = 0;

private:

};