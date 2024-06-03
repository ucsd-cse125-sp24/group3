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

class ServerGameState; // forward declaration to use ptr as parameter

/**
 * @brief Physics struct that contains all movement / collision related data for
 * a particular object
 */
struct Physics {
	/**
	 * @brief constructor for Physics
	 * 
	 * @param movable Whether or not the object is affected by velocity/gravity
	 * @param collider Collision type for this object
	 * @param corner bottom left corner position of the object
	 * @param facing what direction the object is facing
	 * 
	 * NOTE: dimensions is an optional parameter, because most of the time dimensions will be
	 * set by the setModel function!
	 * NOTE: velocity defaults to 0
	 * NOTE: velocityMultitplier defaults to 1
	 * NOTE: Dizziness defaults to 1
	 */
	Physics(bool movable, Collider collider,
		glm::vec3 corner, glm::vec3 facing,
		glm::vec3 dimensions = glm::vec3(1.0f)):
		shared{.corner=corner, .facing=facing, .dimensions=dimensions},
		movable(movable), feels_gravity(true), velocity(glm::vec3(0.0f)), velocityMultiplier(glm::vec3(1.0f)), \
		currTickVelocity(glm::vec3(0.0f)), nauseous(1.0f), collider(collider)
	{}

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
	 * @brief Tick velocity for knockbacks
	 */
	glm::vec3 currTickVelocity;

	/**
	 * @brief Factor for potion of nausea
	 */
	float nauseous;

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
	 * @brief Movable ID (used to index into the movable
	 * objects vector in ServerGameState)
	 */
	MovableID movableID{};

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

	/**
	 * @brief Object's animation state and current action. For non-animated
	 * objects, this defaults as AnimState::IdleAnim.
	 * 
	 */
	AnimState animState;

	/**
	 * @brief Vector of (x, y) positions of GridCells currently occupied by this
	 * object
	 */
	std::vector<glm::ivec2> gridCellPositions;

	/**
	 * @brief Distance moved, for use in deciding when to play footsteps
	 * IMPORTANT: reset every time a footstep sound is performed for objects that play foosteps
	 */
	float distance_moved;

	/**
	 * @param type Type of the object
	 * @param Physics position/physics info for the object
	 * @param modelType What kind of model to render for this object
	 */
	Object(ObjectType type, Physics physics, ModelType modelType);
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

	/**
	 * @brief Code to run when this object collides with another
	 * 
	 * NOTE: default implementation does nothing
	 * only override behaviors will matter
     */
	virtual void doCollision(Object* other, ServerGameState& state) {};


	/*	Debugger Methods	*/

	std::string to_string(unsigned int tab_offset);
	std::string to_string() { return this->to_string(0); }
};

enum class Direction {
    LEFT,
    UP,
    DOWN,
    RIGHT
};

glm::vec3 directionToFacing(const Direction& direction);
