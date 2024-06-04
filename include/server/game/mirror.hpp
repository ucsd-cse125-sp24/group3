#pragma once

#include "server/game/item.hpp"
#include <chrono>

class Mirror : public Item {
public:
	
	/**
	 * @brief Mirror constructor
	 * @param corner Corner position of the Mirror
	 * @param dimensions Dimensions of the Mirror
	*/
	Mirror(glm::vec3 corner, glm::vec3 dimensions);

	/**
	 * @brief Using the mirror causes it to be held for a few seconds
	 * @param other Player object that uses the mirror
	 * @param state Reference to the server's ServerGameState instance
	 * @param itemSelected Mirror's item index in the player's inventory
	*/
	void useItem(Object* other, ServerGameState& state, int itemSelected) override;

	void dropItem(Object* other, ServerGameState& state, int itemSelected, float dropDistance) override;

	/**
	 * @brief Determines whether the mirror has been used for the mirror holding
	 * duration and updated the remaining time it will be used.
	 * @return true if the mirror has been used for the specified duration.
	*/
	bool timeOut();

	/**
	 * @brief Stop holding the mirror.
	 * @note This should be called when the use duration times out OR when the player
	 * selects a different item in their inventory.
	 * @param state Reference to the server's ServerGameState instance
	*/
	void revertEffect(ServerGameState& state);
private:
	/**
	 * @brief The time at which the mirror was last used
	*/
	std::chrono::time_point<std::chrono::system_clock> used_time;

	/**
	 * @brief The last Player that used this mirror object.
	*/
	Player* used_player;
};