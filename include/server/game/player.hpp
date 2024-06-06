#pragma once

#include "server/game/constants.hpp"
#include "server/game/object.hpp"
#include "server/game/creature.hpp"
#include "shared/game/sharedobject.hpp"
#include <vector>

class Player : public Creature {
public:
	SharedPlayerInfo info;
	SharedInventory sharedInventory;
	SharedCompass compass;

	std::vector<SpecificID> inventory;

	/**
	 * @param Corner corner position of the player 
	 * @param facing what direction the player should spawn in facing
	 */
	Player(glm::vec3 corner, glm::vec3 facing);
	~Player();

	virtual SharedObject toShared() override;

	bool canBeTargetted() const;

	/**
	 * @brief This sets the Player as invulnerable to lightning and marks
	 * the timestamp for this event if the value is set to true.
	 * @param isInvulnerable whether the Player should now be invulnerable to lightning
	 * @param duration how long the Player should be invulnerable to lightning (ignored
	 * if isInvulnerable is false)
	 */
	void setInvulnerableToLightning(bool isInvulnerable, double duration);

	/**
	 * @brief Getter for whether this Player is invulnerable to lightning.
	 * @return true if this Player is invulnerable to lightning and false
	 * otherwise.
	 */
	bool isInvulnerableToLightning() const;

	/**
	 * @brief Getter for this Player's lightning invulnerability duration
	 * (this value should be ignored if the Player's invulnerableToLightning
	 * boolean is false)
	 * @return double representing the number of seconds that this Player
	 * should be invulnerable to lightning since the
	 * lightning_invulnerability_start_time timestamp.
	 */
	double getLightningInvulnerabilityDuration() const;

	/**
	 * @brief Returns the timestamp for the last time that this Player became invulnerable
	 * to lightning.
	 * @return std::chrono::time_point<std::chrono::system_clock> timestamp of the last time
	 * this Player became invulnerable to lightning.
	 */
	std::chrono::time_point<std::chrono::system_clock> getLightningInvulnerabilityStartTime() const;

private:
	/**
	 * @brief whether or not the player is sprinting, which is currently only used
	 * to check if the player should resume the running animation or walking animation
	 * upon landing
	 */
	bool is_sprinting;

	/**
	 * @brief Whether or not this Player is currently invulnerable to lightning.
	 * This can occur after a Player used a Mirror to effectively reflect a
	 * lightning bolt (this is to ensure that the player isn't harmed by that
	 * lightning bolt in collisions in subsequent ticks and protects the player
	 * from other lightning bolts in the vicinity for a short time to avoid
	 * a double lightning bolt attack by the DM or something like this).
	 */
	bool invulnerableToLightning;

	/**
	 * @brief Duration, in seconds, of this Player's current lightning invulnerability
	 * (this value should be ignored if invulnerableToLightning is false)
	 */
	double lightningInvulnerabilityDuration;

	/**
	 * @brief Timestamp for the last time this Player gained an invulnerability 
	 * to lightning.
	 * Set by setInvulnerableToLightning().
	 */
	std::chrono::time_point<std::chrono::system_clock> lightning_invulnerability_start_time;
};