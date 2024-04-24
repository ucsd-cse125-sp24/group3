#pragma once

#include "server/game/constants.hpp"
#include "shared/utilities/serialize.hpp"
#include "shared/utilities/serialize_macro.hpp"
#include "server/game/collider.hpp"
#include "shared/game/sharedobject.hpp"
#include "shared/utilities/typedefs.hpp"

//	From sharedobject.hpp
class SharedObject;

//	From sharedobject.hpp
struct SharedPhysics;

/**
 * @brief Physics struct that contains all movement / collision related data for
 * a particular object
 */
struct Physics {
	/**
	 * @brief Shared physics properties (needed by both the server and the 
	 * client)
	 */
	SharedPhysics shared;

	/**
	 * @brief true if the object that contains this Physics struct can move and
	 * false otherwise
	 */
	bool movable;

	/**
	 * @brief 3-D vector that denotes this object's current velocity.
	 */
	glm::vec3 velocity;

	/**
	 * @brief 3-D vector that denotes this object's current acceleration.
	 */
	glm::vec3 acceleration;

	/**
	 * @brief Pointer to this object's collider.
	 */
	Collider* boundary;

	/*	Debugger Methods	*/
	std::string to_string(unsigned int tab_offset);
	std::string to_string() { return this->to_string(0); }
};

class Object {
public:
	/**
	 * @brief Unique object ID (used to index into the ServerGameState::objects
	 * vector)
	 */
	EntityID globalID;

	/**
	 * @brief Type-specific Object ID (used to index into the type-specific
	 * objects vector in ServerGameState)
	 */
	SpecificID typeID;

	/**
	 * @brief Identifies this object's type (derived class)
	 */
	ObjectType type;

	Physics physics;

	Object();
	~Object();

	/**
	 * @brief Generates a SharedObject representation of this object.
	 * @return A SharedObject representation of this object.
	 */
	virtual SharedObject toShared();

	/*	Debugger Methods	*/

	std::string to_string(unsigned int tab_offset);
	std::string to_string() { return this->to_string(0); }
};
