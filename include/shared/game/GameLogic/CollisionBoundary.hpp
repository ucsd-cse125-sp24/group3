#pragma once

#include "constants.hpp"

// Currently box shaped collision
class CollisionBoundary {
public:
	// (x,y,z) center position
	glm::vec3 center;

	// min vector
	glm::vec3 min_position;

	// max vector
	glm::vec3 max_position;

	CollisionBoundary();
	CollisionBoundary(glm::vec3 minPos, glm::vec3 maxPos);
	~CollisionBoundary();

	void detectCollision(CollisionBoundary otherBoundary);
	void resolveCollision(CollisionBoundary otherBoundary);

private:

};