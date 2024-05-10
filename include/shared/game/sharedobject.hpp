#pragma once

#include <optional>
#include <glm/glm.hpp>

//#include "server/game/object.hpp"
#include "shared/utilities/serialize_macro.hpp"
#include "shared/utilities/typedefs.hpp"
#include "shared/game/stat.hpp"

/**
 * @brief An enum for the type of an object; the fields here should match all
 * class names in the inheritance tree in which Object is the root.
 */
enum class ObjectType {
	Object,	//	Generic object type (base class)
	SolidSurface,
	Player,
	Enemy,
	SpikeTrap,
	Potion
};

/**
 * @brief Returns a string representation of the ObjectType enum
 * @param type ObjectType enum
 * @return String representation of the given ObjectType enum
 */
std::string objectTypeString(ObjectType type);

struct SharedStats {
	SharedStats():
		health(0,0,0), speed(0,0,0) {}
	SharedStats(Stat<int> health, Stat<int> speed):
		health(health), speed(speed) {}

	Stat<int> health;
	Stat<int> speed;

	DEF_SERIALIZE(Archive& ar, const unsigned int version) {
		ar & health & speed;
	}
};

/**
 * @brief An enum for the type of an item
 */
enum class ItemType {
	Weapon,
	Spell,
	Potion,
	Blank
};

struct SharedItemInfo {
	bool held; // for rendering
	bool used; // for rendering
	ItemType type;

	DEF_SERIALIZE(Archive& ar, const unsigned int version) {
		ar& type& used& held;
	}
};

enum class SurfaceType {
	Wall,
	Floor,
	Ceiling
};

struct SharedSolidSurface {
	/**
	 * @brief Dimensions of the solid surface in 3 dimensions. The position of
	 * the SolidSurface object is at the center of the object.
	 */
	glm::vec3 dimensions;

	/**
	 * @brief Type of solid surface, e.g. wall, floor, ceiling, etc.(relevant
	 * for rendering)
	 */
	SurfaceType surfaceType;

	DEF_SERIALIZE(Archive& ar, const unsigned int version) {
		ar& dimensions& surfaceType;
	}
};

struct SharedPhysics {
	/**
	 * @brief 3-D vector that denotes this object's current position.
	 */
	glm::vec3 position;

	/**
	 * @brief 3-D vector that denotes this object's bottom left corner 
	 * (min x and z coordinates).
	 */
	glm::vec3 corner;

	/**
	 * @brief 3-D vector that denotes this object's facing direction.
	 */
	glm::vec3 facing;

	/**
	 * @brief 3-D vector that defines a rectangular prism which outlines the
	 * object that contains this SharedPhysic struct.
	 * @note This field dictates the dimensions of the object for rendering in
	 * the client.
	 */
	glm::vec3 dimensions;

	DEF_SERIALIZE(Archive& ar, const unsigned int version) {
		ar& position& corner& facing & dimensions;
	}
};

struct SharedTrapInfo {
	bool triggered;

	DEF_SERIALIZE(Archive& ar, const unsigned int version) {
		ar & triggered;
	}
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

	boost::optional<SharedStats> stats;	
	boost::optional<SharedItemInfo> iteminfo;
	boost::optional<SharedSolidSurface> solidSurface;
	boost::optional<SharedTrapInfo> trapInfo;

	SharedObject() {} // cppcheck-suppress uninitMemberVar
	~SharedObject() {}
	 
	DEF_SERIALIZE(Archive& ar, const unsigned int version) {
		ar& globalID & type& physics & stats & iteminfo & solidSurface & trapInfo;
	}
private:
};