#pragma once

#include <optional>

#include "server/game/object.hpp"
#include "shared/utilities/serialize_macro.hpp"
#include "server/game/creature.hpp" // stats

struct SharedPhysics {
	/**
	 * @brief 3-D vector that denotes this object's current position.
	 */
	glm::vec3 position;

	/**
	 * @brief 3-D vector that denotes this object's facing direction.
	 */
	glm::vec3 facing;
};

/**
 * @brief Representation of the Object class used by ServerGameState, containing
 * exactly the subset of Object data required by the client.
 */
class SharedObject {
public:
	unsigned int globalID;
	unsigned int typeID;
	ObjectType type;
	SharedPhysics physics;

	std::optional<Stats> stats;	

	SharedObject() {}
	~SharedObject() {}

	DEF_SERIALIZE(Archive& ar, const unsigned int version) {
		ar& globalID& typeID& type& physics.position & physics.facing;
	}
private:
};