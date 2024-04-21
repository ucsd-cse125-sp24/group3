#pragma once

#include "constants.hpp"
#include "shared/utilities/serialize.hpp"
#include "shared/utilities/serialize_macro.hpp"
#include "collider.hpp"

//	From sharedobject.hpp
class SharedObject;

//	From sharedobject.hpp
struct SharedPhysics;

/**
 * @brief An enum for the type of an object; the fields here should match all
 * class names in the inheritance tree in which Object is the root.
 */
enum class ObjectType {
	Object	//	Generic object type (base class)
};

/**
 * @brief Physics struct that contains all movement / collision related data for
 * a particular object
 */
struct Physics {
	/**
	 * @brief true if the object that contains this Physics struct can move and
	 * false otherwise
	 */
	bool movable;

	/**
	 * @brief 3-D vector that denotes this object's current position.
	 */
	glm::vec3 position;

	/**
	 * @brief 3-D vector that denotes this object's current velocity.
	 */
	glm::vec3 velocity;

	/**
	 * @brief 3-D vector that denotes this object's current acceleration.
	 */
	glm::vec3 acceleration;

	/**
	 * @brief 3-D vector that denotes this object's facing direction.
	 */
	glm::vec3 facing;

	/**
	 * @brief Pointer to this object's collider.
	 */
	Collider* boundary;

	/**
	 * @brief Generates a SharedPhysics representation of this Physics struct.
	 * @return SharedPhysics representation of this Physics struct.
	 */
	SharedPhysics generateSharedPhysics();
};

class Object {
public:
	/**
	 * @brief Unique object ID (used to index into the ServerGameState::objects
	 * vector)
	 */
	unsigned int globalID;

	/**
	 * @brief Type-specific Object ID (used to index into the type-specific
	 * objects vector in ServerGameState)
	 */
	unsigned int typeID;

	/**
	 * @brief Identifies this object's type (derived class)
	 */
	ObjectType type;

	//	TODO: Add Physics struct that contains movement vectors + collider info
	Physics physics;

	// (x,y,z) position
	//glm::vec3 position;

	// velocity vector
	//glm::vec3 velocity;

	// acceleration vector
	//glm::vec3 acceleration;

	//Collision boundary of the object
	//Collider* boundary;

	Object();
	~Object();

	//virtual void applyAction();
	//virtual void onCollision(Object* other);

	/**
	 * @brief Generates a SharedObject representation of this object.
	 * @return Generates a SharedObject representation of this object.
	 */
	SharedObject generateSharedObject();

	/*	Debugger Methods	*/

	std::string to_string(unsigned int tab_offset);
	std::string to_string() { return this->to_string(0); }

	/*void setPosition(const glm::vec3& newPos) {
		position = newPos;
	}

	void setVelocity(const glm::vec3& newVel) {
		velocity = newVel;
	}

	void setAcceleration(const glm::vec3& newAcc) {
		acceleration = newAcc;
	}

    DEF_SERIALIZE(Archive& ar, const unsigned int version) {
        ar & id & position & acceleration;
    }*/

private:
};