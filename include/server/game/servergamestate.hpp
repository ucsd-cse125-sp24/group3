#pragma once

#include "shared/utilities/typedefs.hpp"
#include "shared/game/sharedgamestate.hpp"
#include "shared/utilities/config.hpp"
#include "shared/utilities/smartvector.hpp"
#include "server/game/object.hpp"
#include "server/game/objectmanager.hpp"
#include "shared/game/event.hpp"

#include <string>
#include <vector>
#include <chrono>
#include <unordered_map>
#include <queue>

//	From sharedgamestate.hpp
//struct SharedGameState;

/// Represents a list of events from a certain client with a specified ID
using EventList = std::vector<std::pair<EntityID, Event>>;

/**
 * @brief The ServerGameState class contains all abstract game state data and
 * logic for a single game state instance (i.e., for one match played by 4
 * players).
 */
class ServerGameState {
public:
	/**
	 * @brief ObjectManager instance that manages all objects in this game
	 * instance at the current timestep.
	 */
	ObjectManager objects;

	/**
	 * @brief Creates a ServerGameState instance. The intial GamePhase is set to
	 * Lobby.
	 */
	ServerGameState();

	/**
	 * @brief Creats a ServerGameState instance and sets the initial game phase
	 * to the given GamePhase.
	 * @param start_phase GamePhase that the new ServerGameState instance will
	 * start in.
	 */
	explicit ServerGameState(GamePhase start_phase);

	ServerGameState(GamePhase start_phase, GameConfig config);

	~ServerGameState();

	/**
	 * @brief Updates this ServerGameState from the current timestep to the
     * next one.
	 */
	void update(const EventList& events);

	//	TODO: Add specific update methods (E.g., updateMovement() to update
	//	object movement)

	void updateMovement();

	//	TODO: Add implementations of items

	void useItem();

	/*	SharedGameState generation	*/

	//	TODO: Modify this function to dynamically allocate a SharedGameState
	//	instance and return a pointer (perhaps a unique pointer) to it
	/**
	 * @brief Generate a SharedGameState object from this ServerGameState
	 * instance.
	 * @return ShareGameState instance that represents this ServerGameState
	 * instance.
	 */
	SharedGameState generateSharedGameState();

	/*	Other getters and setters	*/

	/**
	 * @brief Returns the current timestep of this ServerGameState instance.
	 * @return The current timestep of this ServerGameState instance.
	 */
	unsigned int getTimestep() const;

	/**
	 * @brief Returns the timestep length of this ServerGameState instance.
	 * @return Timestep length (in milliseconds)
	 */
	std::chrono::milliseconds getTimestepLength() const;

	/**
	 * @brief Returns the phase that this ServerGameState instance is currently
	 * in.
	 * @return The current GamePhase of this ServerGameState instance. 
	 */
	GamePhase getPhase() const;

	/**
	 * @brief setter for game phase
	 */
	void setPhase(GamePhase phase);

	/**
	 * Reassign id to the specified name in the mapping. This is okay to call if the
	 * player is already in the mapping, as nothing will happen. If a player's name
	 * has changed, then this will update their name as well.
	 */
	void addPlayerToLobby(EntityID id, const std::string& name);
	/**
	 * Removes a player from the lobby with the specified id.
	 */
	void removePlayerFromLobby(EntityID id);
	/**
	 * Getter for the mapping between entity ID and player name in the lobby
	 */
	const std::unordered_map<EntityID, std::string>& getLobbyPlayers() const;

	/**
	 * Getter for the lobby name
	 */
	const std::string& getLobbyName() const;
	/**
	 * Returns how many max players can be in the lobby, based on the config option
	 */
	int getLobbyMaxPlayers() const;

	/*	Debugger Methods	*/

	/**
	 * @brief Creates a string representation of this ServerGameState object.
	 * @return A string representation of this ServerGameState object.
	 */
	std::string to_string();

private:
	/**
	 *  Timestep length in milliseconds.
	 */
	std::chrono::milliseconds timestep_length;

	/**
	 *  Current timestep (starts at 0)
	 */
	unsigned int timestep;

	/**
	 * @brief Lobby information regarding the players that are taking part in
	 * this game instance.
	 */
	Lobby lobby;

	/**
	 * @brief The current phase of this game instance.
	 */
	GamePhase phase;

	//	TODO: Add reference to passed-in Event queue.
};