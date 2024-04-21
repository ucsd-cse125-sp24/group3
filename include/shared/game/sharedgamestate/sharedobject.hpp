#pragma once

#include "shared/game/gamelogic/object.hpp"
#include "shared/utilities/serialize_macro.hpp"

/**
 * @brief Representation of the Physics struct used by the Object class,
 * containing exactly the subset of Physics data required by the client.
 */
struct SharedPhysics {
	glm::vec3 position;
	glm::vec3 facing;

	DEF_SERIALIZE(Archive& ar, const unsigned int version) {
		ar& position& facing;
	}
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

	SharedObject() {}
	~SharedObject() {}

	DEF_SERIALIZE(Archive& ar, const unsigned int version) {
		ar& globalID& typeID& type& physics;
	}
private:
};