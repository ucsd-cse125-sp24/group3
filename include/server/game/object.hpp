#pragma once

#include "server/game/constants.hpp"
#include "shared/utilities/serialize.hpp"
#include "shared/utilities/serialize_macro.hpp"
#include "server/game/collider.hpp"
#include "shared/game/sharedobject.hpp"
#include "shared/utilities/typedefs.hpp"
#include "shared/game/sharedmodel.hpp"

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
	 * @brief true if the object that contains this Physics struct feels gravity and
	 * false otherwise
	 */
	bool feels_gravity;

	/**
	 * @brief 3-D vector that denotes this object's current velocity.
	 */
	glm::vec3 velocity;

	/**
	 * @brief 3-D vector that denotes this object's velocity multiplier.
	 */
	glm::vec3 velocityMultiplier;

	/**
	 * @brief This object's collider type.
	 */
	Collider collider;

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
	EntityID globalID {};

	/**
	 * @brief Type-specific Object ID (used to index into the type-specific
	 * objects vector in ServerGameState)
	 */
	SpecificID typeID {};

	/**
	 * @brief Identifies this object's type (derived class)
	 */
	ObjectType type;

	/**
	 * @brief Object's Physics-related properties
	 */
	Physics physics;

	/**
	 * @brief Object's render model type (specifies this Object's render model
	 * to the client)
	 */
	ModelType modelType;

	explicit Object(ObjectType type);
	virtual ~Object();

	/**
	 * @brief Sets this Object's model and initializes its dimensions to the
	 * given model's default dimensions.
	 * @param type ModelType of the render model that this Object should be
	 * rendered as.
	 */
	void setModel(ModelType type);

	/**
	 * @brief Maps from ModelType to a model's dimensions as read from the model
	 * files. (At present, these values are hard-coded in object.cpp)
	 */
	static std::unordered_map<ModelType, glm::vec3> models;

	/**
	 * @brief Generates a SharedObject representation of this object.
	 * @return A SharedObject representation of this object.
	 */
	virtual SharedObject toShared();

	/*	Debugger Methods	*/

	std::string to_string(unsigned int tab_offset);
	std::string to_string() { return this->to_string(0); }
};
