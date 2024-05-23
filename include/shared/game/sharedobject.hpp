#pragma once

#include <optional>
#include <algorithm>
#include <glm/glm.hpp>

#include "shared/utilities/serialize_macro.hpp"
#include "shared/utilities/typedefs.hpp"
#include "shared/game/stat.hpp"
#include "shared/game/sharedmodel.hpp"
#include "shared/game/celltype.hpp"
#include "shared/game/status.hpp"
#include <chrono>

/**
 * @brief An enum for the type of an object; the fields here should match all
 * class names in the inheritance tree in which Object is the root.
 */
enum class ObjectType {
	Object,	//	Generic object type (base class)
	SolidSurface,
	Potion,
	Player,
	Enemy,
    Torchlight,
	SpikeTrap,
	DungeonMaster,
	FireballTrap,
	Projectile,
	FloorSpike,
	FakeWall,
	ArrowTrap,
	TeleporterTrap,
	Spell,
	Slime,
	Item,
	Exit,
	Orb,
	Weapon,
	WeaponCollider,
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
	SharedStats(Stat<int>&& health, Stat<int>&& speed):
		health(health), speed(speed) {}

	Stat<int> health;
	Stat<int> speed;

	DEF_SERIALIZE(Archive& ar, const unsigned int version) {
		ar & health & speed;
	}
};

struct SharedStatuses {
public:
	SharedStatuses() = default;

	/**
	 * @param st Status to add
	 * @param len how many ticks it should be active
	 */
	void addStatus(Status st, size_t len);

	/**
	 * For the GUI, returns a list of every status string.
	 * 
	 * @returns All status strings, or "Unafflicted" if there are no statuses
	 */
	std::vector<std::string> getStatusStrings() const;

	/**
	 * counts down all status lengths by one tick
	 */
	void tickStatus();

	/**
	 * @param st Status to check for
	 * @returns The amount of ticks remaining for a particular status. Returns 0 if the status is not applied
	 */
	size_t getStatusLength(Status st) const;

	DEF_SERIALIZE(Archive& ar, const unsigned int version) {
		ar & map;
	}

private:
	std::unordered_map<Status, size_t> map;
};


using UsedItemsMap = std::unordered_map<SpecificID, std::pair<ModelType, double>>;

struct SharedInventory {
	// need to share itemtype data...
	int selected;
	int inventory_size;
	std::vector<ModelType> inventory;	// For GUI model
	std::vector<int> usesRemaining;		// For GUI text description
	UsedItemsMap usedItems;				// For GUI effect duration

	/**
	 * @brief Denotes whether this player is carrying the Orb in their inventory
	 */
	bool hasOrb;

	DEF_SERIALIZE(Archive& ar, const unsigned int version) {
		ar& selected& inventory_size& inventory& usedItems & hasOrb& usesRemaining;
	}
}; 

struct SharedTrapInventory {
	// need to share itemtype data...
	int selected;
	int inventory_size;
	std::vector<ModelType> inventory;
	std::unordered_map<CellType, std::time_t> trapsInCooldown;

	DEF_SERIALIZE(Archive& ar, const unsigned int version) {
		ar& selected& inventory_size& inventory& trapsInCooldown;
	}
};

struct SharedItemInfo {
	bool held; // for rendering
	bool used; // for rendering
	double remaining_time;

	DEF_SERIALIZE(Archive& ar, const unsigned int version) {
		ar& used& held& remaining_time;
	}
};

struct SharedWeaponInfo {
	bool attacked;

	DEF_SERIALIZE(Archive& ar, const unsigned int version) {
		ar& attacked;
	}
};

enum class SurfaceType {
	Wall,
	Floor,
	Ceiling,
    Pillar,
};

struct SharedSolidSurface {
	/**
	 * @brief Type of solid surface, e.g. wall, floor, ceiling, etc.(relevant
	 * for rendering)
	 */
	SurfaceType surfaceType;

	bool dm_highlight;

	DEF_SERIALIZE(Archive& ar, const unsigned int version) {
		ar & surfaceType & dm_highlight;
	}
};

struct SharedPhysics {
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

	/**
	 * @brief Calculates and returns the center position of this object.
	 * @return glm::vec3 that denotes the center position of this object.
	 */
	glm::vec3 getCenterPosition() const;

	DEF_SERIALIZE(Archive& ar, const unsigned int version) {
		ar& corner& facing & dimensions;
	}
};

struct SharedTrapInfo {
	bool triggered;

	DEF_SERIALIZE(Archive& ar, const unsigned int version) {
		ar & triggered;
	}
};

struct SharedPlayerInfo {
	bool is_alive;
	time_t respawn_time; // unix timestamp in ms when the player will be respawned
	bool render; // for invis potion

	DEF_SERIALIZE(Archive& ar, const unsigned int version) {
		ar & is_alive & respawn_time & render;
	}
};

struct SharedPointLightInfo {
    float intensity;
    // lighting properties of that the light source
    // emits
    glm::vec3 ambient_color;
    glm::vec3 diffuse_color;
    glm::vec3 specular_color; // shiny effects
    // these two affect the dropoff of the light intensity
    // as distance increases
    float attenuation_linear;
    float attenuation_quadratic;

	DEF_SERIALIZE(Archive& ar, const int version) {
		ar & intensity & ambient_color & diffuse_color & specular_color &
            attenuation_linear & attenuation_quadratic;
    }
};

struct SharedExit {
	/**
	 * @brief Whether the given exit is open or closed
	 */
	bool open;

	DEF_SERIALIZE(Archive& ar, const unsigned int version) {
		ar & open;
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
	ModelType modelType;

	boost::optional<SharedStats> stats;
	boost::optional<SharedItemInfo> iteminfo;
	boost::optional<SharedSolidSurface> solidSurface;
	boost::optional<SharedTrapInfo> trapInfo;
	boost::optional<SharedPlayerInfo> playerInfo;
	boost::optional<SharedInventory> inventoryInfo;
	boost::optional<SharedTrapInventory> trapInventoryInfo;
    boost::optional<SharedPointLightInfo> pointLightInfo;
	boost::optional<SharedStatuses> statuses;
	boost::optional<SharedExit> exit;
	boost::optional<SharedWeaponInfo> weaponInfo;

	SharedObject() {} // cppcheck-suppress uninitMemberVar
	~SharedObject() {}
	 
	DEF_SERIALIZE(Archive& ar, const unsigned int version) {
		ar & globalID & type & physics & modelType & stats & 
			 iteminfo & solidSurface & trapInfo & playerInfo & 
			 inventoryInfo & statuses & trapInventoryInfo & pointLightInfo &
             exit & weaponInfo;
	}
private:
};
