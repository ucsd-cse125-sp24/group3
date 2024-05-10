#pragma once

#include "server/game/constants.hpp"

struct Physics;

/**
 * @brief Enumeration to identify collider shape
 */
enum class Collider {
	//	A collider type of None indicates that an object does not have a
	//	collider - all collision checks are ignored
	None,	
	Box,
	Sphere
};

/** 
 * @brief  Detects whether a collision has occurred between two objects given
 * their Physics structs; this is done by checking for overlap between the two
 * object's surfaces (defined by their positions + dimensions + collider shape)
 * @param obj1  Reference to the first object's Physics struct.
 * @param obj2  Reference to the second object's Physics struct.
 * @return  true if a collision (overlap) is detected between the two objects'
 * surfaces, and false otherwise.
 */
bool detectCollision(Physics& obj1, Physics& obj2);

/** 
 * @brief  Detects whether a collision has occurred between two objects given
 * their Physics structs and that the first object is a sphere.
 * @param sphere Reference to the sphere object's Physics struct.
 * @param obj  Reference to the second object's Physics struct.
 * @return  true if a collision is detected between the sphere and the second
 * object, and false otherwise.
 */
bool detectCollisionSphere(Physics& sphere, Physics& obj);

/**
 * @brief  Detects whether a collision has occurred between two objects given
 * their Physics structs and that the first object is a box.
 * @param box Reference to the box object's Physics struct.
 * @param obj  Reference to the second object's Physics struct.
 * @return  true if a collision is detected between the box and the second
 * object, and false otherwise.
 */
bool detectCollisionBox(Physics& box, Physics& obj);