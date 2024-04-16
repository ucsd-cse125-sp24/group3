#pragma once

#include "constants.hpp"
#include "shared/utilities/vector3.hpp"
#include "Collider.hpp"

class Object {
public:
	// Unique ID for the objects
	unsigned int id;

	// (x,y,z) position
	glm::vec3 position;

	// velocity vector
	glm::vec3 velocity;

	// acceleration vector
	glm::vec3 acceleration;

	//Collision boundary of the object
	Collider* boundary;

	Object();
	~Object();

	//virtual void applyAction();
	//virtual void onCollision(Object* other);

	std::string to_string(unsigned int tab_offset);
	std::string to_string() { return this->to_string(0); }

	void setPosition(const glm::vec3& newPos) {
		position = newPos;
	}

	void setVelocity(const glm::vec3& newVel) {
		velocity = newVel;
	}

	void setAcceleration(const glm::vec3& newAcc) {
		acceleration = newAcc;
	}

private:
	static unsigned int numObjects;
};