#pragma once

#include "shared/utilities/typedefs.hpp"
#include "shared/game/sharedgamestate.hpp"
#include "shared/utilities/config.hpp"
#include "shared/utilities/smartvector.hpp"
#include "server/game/object.hpp"
#include "server/game/objectmanager.hpp"
#include "shared/game/event.hpp"
#include "server/game/grid.hpp"

#include <string>
#include <vector>
#include <chrono>
#include <unordered_map>
#include <queue>

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

	ServerGameState(GamePhase start_phase, const GameConfig& config);

	/**
	 * @brief This is the ONLY constructor that initializes the maze from a
	 * maze_file argument. All other constructors must call it.
	 * @param maze_file Name of maze file to load. (should be in maps/
	 * directory).
	 */
	explicit ServerGameState(GameConfig config);

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

	void updateTraps();

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

	const Lobby& getLobby() const;

	/*	Maze initialization	*/
	
	/**
	 * @brief Reads from maze file and initializes this ServerGameState's
	 * Grid instance, as well as creating all necessary environment objects.
	 */
	void loadMaze();

	/*	Maze getters	*/

	/**
	 * @brief Returns a reference to the Grid in use by this ServerGameState
	 * @return Reference to Grid instance used by this ServerGameState instance
	 */
	Grid& getGrid();

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

	/**
	 * @brief Name of maze file that the server should load.
	 */
	std::string maze_file;

	/**
	 * @brief Name of the directory that contains maze map files.
	 */
	std::string maps_directory;

	/**
	 * @brief 2-D Grid of GridCells (filled after loadMaze() is called).
	 */
	Grid grid;
};