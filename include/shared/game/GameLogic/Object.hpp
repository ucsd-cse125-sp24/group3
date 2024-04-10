#pragma once

#include "constants.hpp"
#include "CollisionBoundary.hpp"

class Object {
public:
	// Unique ID for the objects
	unsigned int entityID;

	// (x,y,z) position
	glm::vec3 position;

	// direction vector
	glm::vec3 direction;

	// Collision boundary of the object
	CollisionBoundary boundary;

	Object();
	~Object();

	virtual void applyAction();
	virtual void onCollision(Object* other);

private:

};