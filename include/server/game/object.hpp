#pragma once

#include "constants.hpp"
#include "shared/utilities/serialize.hpp"
#include "shared/utilities/serialize_macro.hpp"
#include "collider.hpp"

//	From sharedobject.hpp
class SharedObject;

/**
 * @brief An enum for the type of an object; the fields here should match all
 * class names in the inheritance tree in which Object is the root.
 */
enum class ObjectType {
	Object	//	Generic object type (base class)
};

/**
 * @brief Returns a string representation of the ObjectType enum
 * @param type ObjectType enum
 * @return String representation of the given ObjectType enum
 */
std::string objectTypeString(ObjectType type);

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

	Physics physics;

	Object();
	~Object();

	/**
	 * @brief Generates a SharedObject representation of this object.
	 * @return Generates a SharedObject representation of this object.
	 */
	virtual SharedObject generateSharedObject();

	/*	Debugger Methods	*/

	std::string to_string(unsigned int tab_offset);
	std::string to_string() { return this->to_string(0); }
};
