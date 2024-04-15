#pragma once

#include "constants.hpp"
#include "shared/utilities/vector3.hpp"
#include "CollisionBoundary.hpp"

class Object {
public:
	// Unique ID for the objects
	//unsigned int entityID;

	// (x,y,z) position
	//glm::vec3 position;

	// direction vector
	//glm::vec3 direction;

	// Collision boundary of the object
	//CollisionBoundary boundary;

	unsigned int id;

	//	Can modify these into glm::vec3, currently using these so that the
	//	debugger will compile
	Vector3 position;
	Vector3 velocity;
	Vector3 acceleration;

	Object();
	~Object();

	//virtual void applyAction();
	//virtual void onCollision(Object* other);

	std::string to_string(unsigned int tab_offset);
	std::string to_string() { return this->to_string(0); }

private:
	static unsigned int numObjects;
};