#pragma once

#include <optional>
#include <glm/glm.hpp>

//#include "server/game/object.hpp"
#include "shared/utilities/serialize_macro.hpp"
#include "shared/utilities/typedefs.hpp"

/**
 * @brief An enum for the type of an object; the fields here should match all
 * class names in the inheritance tree in which Object is the root.
 */
enum class ObjectType {
	Object,	//	Generic object type (base class)
	Player,
	Enemy
};

/**
 * @brief Returns a string representation of the ObjectType enum
 * @param type ObjectType enum
 * @return String representation of the given ObjectType enum
 */
std::string objectTypeString(ObjectType type);

struct Stats {
	float health;
	float speed;
};

struct ItemInfo {
	enum ItemType { healing, swiftness, invisible, key };

	bool held; // for rendering
	bool used;
	float scalar;
	float timer;
	ItemType type;
};

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
	EntityID globalID;
	ObjectType type;
	SharedPhysics physics;

	std::optional<Stats> stats;	
	std::optional<ItemInfo> iteminfo;

	SharedObject() {} // cppcheck-suppress uninitMemberVar
	~SharedObject() {}

	DEF_SERIALIZE(Archive& ar, const unsigned int version) {
		ar& globalID & type& physics.position & physics.facing;
	}
private:
};