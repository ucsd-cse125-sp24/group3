#pragma once

#include "server/game/constants.hpp"
#include "server/game/object.hpp"
#include "server/game/creature.hpp"
#include "shared/game/sharedobject.hpp"
#include <chrono>

class Weapon;

class DungeonMaster : public Creature {
public:
	SharedTrapInventory sharedTrapInventory;
	SharedDMInfo dmInfo;
	Weapon* lightning;

	DungeonMaster(glm::vec3 corner, glm::vec3 facing);
	~DungeonMaster();

	virtual SharedObject toShared() override;

	int getPlacedTraps();

	void setPlacedTraps(int placedTraps);

	// For lightning usage
	void useMana();
	void manaRegen();

	/**
	 * @brief Sets the whether the DungeonMaster is paralyzed. If isParalyzed
	 * is true, then this sets the paralysis duration and marks the timestamp for
	 * the paralysis start event.
	 * @param isParalyzed Whether the DungeonMaster should now be paralyzed.
	 * @param paralysis_duration How long the DungeonMaster should be paralyzed for
	 * (ignored if isParalyzed is false)
	 */
	void setParalysis(bool isParalyzed, double paralysis_duration);

	/**
	 * @brief Getter for whether the DungeonMaster is paralyzed.
	 * @return true if the DungeonMaster is paralyzed and false otherwise.
	 */
	bool isParalyzed() const;

	/**
	 * @brief Getter for the DungeonMaster's paralysis duration.
	 * (this value should be ignored if the DungeonMaster's paralyzed boolean
	 * is false)
	 * @return double representing the number of seconds that the DungeonMaster
	 * should be paralyzed since the paralysis_start_time timestamp.
	 */
	double getParalysisDuration() const;

	/**
	 * @brief Getter for the timestamp of the last time the DungeonMaster was
	 * paralyzed.
	 * @return std::chrono::time_point<std::chrono::system_clock> timestamp of
	 * the last time the DungeonMaster became paralyzed.
	 */
	std::chrono::time_point<std::chrono::system_clock> getParalysisStartTime() const;
		
private:
	/**
	 * @brief Duration, in seconds, of the Dungeon Master's current paralysis 
	 * (this value should be ignored if paralyzed is false)
	 */
	double paralysisDuration;

	/**
	 * @brief Timestamp for the last time the DungeonMaster became paralyzed.
	 * Set by setParalysis().
	 */
	std::chrono::time_point<std::chrono::system_clock> paralysis_start_time;

	int placedTraps;
	std::chrono::system_clock::time_point mana_used;
};