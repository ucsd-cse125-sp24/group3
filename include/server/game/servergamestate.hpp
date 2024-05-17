#pragma once

#include "shared/utilities/typedefs.hpp"
#include "shared/game/sharedgamestate.hpp"
#include "shared/utilities/config.hpp"
#include "shared/utilities/smartvector.hpp"
#include "shared/utilities/custom_hash.hpp"
#include "server/game/object.hpp"
#include "shared/game/event.hpp"
#include "server/game/grid.hpp"
#include "server/game/objectmanager.hpp"

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

	/**
	 * @brief tell the gamestate to delete this entity at the end of the tick
	 */
	void markForDeletion(EntityID id);

	//	TODO: Add specific update methods (E.g., updateMovement() to update
	//	object movement)

	void updateMovement();

	/**
	 * @brief Detects whether a collision occurs with other objects when the
	 * given object moves to the given position.
	 * @param object Object to perform collision detection for
	 * @param newCornerPosition Corner position object moves to at which 
	 * collision detection is performed
	 * @note This method moves the object to the given position - i.e., it
	 * updates the object's gridCellPositions vector and the ObjectManager's
	 * cellToObjects unordered_map.
	 * @return true if the object overlaps (collides) with any other object that
	 * has a collider at the given position, and false otherwise.
	 */
	bool hasObjectCollided(Object* object, glm::vec3 newCornerPosition);

	//	TODO: Add implementations of items
	void updateItems();

	void doProjectileTicks();

	void updateTraps();

	void handleDeaths();

	void handleRespawns();

	void deleteEntities();

	/*	SharedGameState generation	*/

	//	TODO: Modify this function to dynamically allocate a SharedGameState
	//	instance and return a pointer (perhaps a unique pointer) to it
	/**
	 * @brief Generate a SharedGameState object from this ServerGameState
	 * instance.
	 * @param send_all True if you should send a 100% update to the client, false
	 * if you should just send the updated objects
	 * 
	 * NOTE: if send_all is false and you generate an update based on the diffs, this
	 * function will clear the updated_entities unordered_set for you
	 * 
	 * @return vector of partial ShareGameState instances that represent different pieces
	 * of the SharedGameState instance
	 */
	std::vector<SharedGameState> generateSharedGameState(bool send_all);

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
	 * @brief Returns the match phase that this ServerGameState instance is
	 * currently in.
	 * @return The current MatchPhase of this ServerGameState instance.
	 */
	MatchPhase getMatchPhase() const;

	/**
	 * @brief Sets this ServerGameState's match phase to the given match phase.
	 * @param phase New MatchPhase for this ServerGameState instance.
	 */
	void setMatchPhase(MatchPhase phase);

	/**
	 * @brief Sets the playerVictory boolean
	 * @param playerVictory boolean value to set the playerVictory value to
	 */
	void setPlayerVictory(bool playerVictory);

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
	void loadMaze(const Grid& grid);

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
	 * list of entities to delete at the end of the tick
     */
	std::unordered_set<EntityID> entities_to_delete;

	/**
	 * list of entities that have been changed, so we only have to include
	 * these in partial updates to the clients 
	 */
	std::unordered_set<EntityID> updated_entities;

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
	 * @brief The current match phase of this game instance - at the start of
	 * the game, this is MatchPhase::MazeExploration
	 */
	MatchPhase matchPhase;

	/**
	 * @brief Amount of time, in milliseconds, left until the end of the match
	 * This value only becomes relevant when matchPhase is set to
	 * MatchPhase::RelayRace
	 */
	std::chrono::milliseconds time_left;

	/**
	 * @brief Player victory is by default false - only becomes true if a Player
	 * collides with an open exit while holding the Orb
	 */
	bool playerVictory;

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

	/**
	 * @brief Set of pairs of pointers to Objects that have collided in the
	 * current timestep.
	 * Maintained by hasObjectCollided() (which adds object pairs to it upon
	 * collision detection) and updateMovement() (which clears it)
	 */
	std::unordered_set<std::pair<Object*, Object*>, pair_hash> collidedObjects;
};