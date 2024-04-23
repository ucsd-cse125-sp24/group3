#pragma once

#include "shared/game/gamelogic/object.hpp"
#include "shared/utilities/serialize_macro.hpp"

struct Stats;

/**
 * @brief Representation of the Object class used by ServerGameState, containing
 * exactly the subset of Object data required by the client.
 */
class SharedObject {
public:
	unsigned int globalID;
	unsigned int typeID;
	ObjectType type;
	glm::vec3 position;
	glm::vec3 facing;

	std::optional<Stats> stats;	

	SharedObject() {}
	~SharedObject() {}

	DEF_SERIALIZE(Archive& ar, const unsigned int version) {
		ar& globalID& typeID& type& position & facing;
	}
private:
};