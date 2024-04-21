#pragma once

#include "shared/utilities/typedefs.hpp"
#include "shared/utilities/config.hpp"
#include "shared/utilities/smartvector.hpp"
#include "shared/game/gamelogic/object.hpp"

#include <string>
#include <vector>
#include <chrono>
#include <unordered_map>
#include <queue>

//	From sharedgamestate.hpp
struct SharedGameState;

enum class GamePhase {
	TITLE_SCREEN,
	LOBBY,
	GAME
};

/**
 * @brief Information about the current lobby of players.
 */
struct Lobby {
	//	TODO: Perhaps instead of a mapping from EntityID -> string, the mapping
	//	could eventually be EntityID -> Player (where Player derives from
	//	Object)?

	/**
	 * @brief A hash table that maps from player's EntityID to their names.
	 */
	std::unordered_map<EntityID, std::string> players;

	/**
	 * @brief The maximum number of players that this game instance can support.
	 */
	int max_players;

	//	TODO: Add a player role listing? I.e., which player is playing which
	//	character and which player is playing as the Dungeon Master?
};

/*	Constants	*/
#define	FIRST_TIMESTEP	0
#define TIMESTEP_LEN	std::chrono::milliseconds(30)
#define	MAX_PLAYERS		4

/**
 * @brief The ServerGameState class contains all abstract game state data and
 * logic for a single game state instance (i.e., for one match played by 4
 * players).
 */
class ServerGameState {
public:
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
	ServerGameState(GamePhase start_phase);

	ServerGameState(GamePhase start_phase, GameConfig config);

	~ServerGameState();

	/**
	 * @brief Updates this ServerGameState from the current timestep to the
     * next one.
	 */
	void update();

	//	TODO: Add specific update methods (E.g., updateMovement() to update
	//	object movement)

	void updateMovement();

	/*	SharedGameState generation	*/

	/**
	 * @brief Generate a SharedGameState object from this ServerGameState
	 * instance.
	 * @return ShareGameState instance that represents this ServerGameState
	 * instance.
	 */
	SharedGameState generateSharedGameState();

	/*	Object CRUD methods	*/
	
	/**
	 * @brief Creates a new object with the specified type and places it in the
	 * relevant type-specific object vector.
	 * 
	 * This method assigns a unique global ID and type-specific ID to the newly
	 * created object (NOTE: the Object constructor does NOT do this!)
	 * 
	 * @return the type-specific ID of the created object (the idea is that more
	 * often than not, you'd want to get a reference to the derived class of the
	 * newly created object than a reference to Object which you'd then need to
	 * cast)
	 */
	unsigned int createObject(ObjectType type);

	/**
	 * @brief Attempts to remove an object with the given global ID.
	 * 
	 * This removes the object from the type-specific object vector that
	 * contains it as well as from the global Object & vector.
	 * 
	 * @param global_id Global ID of the object to remove from this game
	 * instance.
	 * @return true if the object was successfully removed and false otherwise.
	 */
	bool removeObject(unsigned int global_id);

	/**
	 * @brief Attempts to retrieve the object with the given global ID.
	 * @param global_id Global ID of the object to retrieve
	 * @return A pointer to the object with the given global ID or nullptr if
	 * none exists.
	 */
	Object* getObject(unsigned int global_id);

	//	TODO: Add type-specific object getters (e.g., getPlayer(), getWalls(),
	//	etc.)

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
	 * Reassign id to the specified name in the mapping. This is okay to call if the
	 * player is already in the mapping, as nothing will happen. If a player's name
	 * has changed, then this will update their name as well.
	 */
	void addPlayerToLobby(EntityID id, std::string name);
	/**
	 * Removes a player from the lobby with the specified id.
	 */
	void removePlayerFromLobby(EntityID id);
	/**
	 * Getter for the mapping between entity ID and player name in the lobby
	 */
	const std::unordered_map<EntityID, std::string>& getLobbyPlayers() const;
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
	/*
	 * Note on how Objects are stored:
	 *
	 * The ServerGameState class has a vector<Object &> objects array that holds
	 * references to all Objects in the game instance at the current timestep.
	 * The actual objects are stored in type-specific vectors, e.g., all of the
	 * Player objects are stored in vector<Player> players, and all Wall objects
	 * are stored in vector<Wall> walls; this idea requires that each object is
	 * stored in exactly one type-specific object vector. I think a good rule is
	 * that an object should be stored in the type-specific array whose type is
	 * closest to its actual type. E.g., let's say Creature derives from Object
	 * and Player and Enemy derive from Creature, and that there exists a
	 * vector<Object> base_objects type-specific object vector as well as a 
	 * vector<Creature> creatures type-specific object vector. All Player, 
	 * Enemy, and Creature objects should be stored in the creatures vector and 
	 * all other objects should be stored in the base_objects vector.
	 * 
	 * To access an object in the vector<Object &> vector, use its global ID as
	 * the index.
	 * To access an object in the type-specific vector that contains it, use
	 * its type-specific ID as the index.
	 * Hence, all Objects have two IDs - a global ID and a type-specific ID.
	 */

	//	TODO: Refactor vector<T> array + queue<unsigned int> freelist into a
	//	single template class - otherwise, each type-specific object vector will
	//	come with an extra free list and this will get hairy to use.

	SmartVector<Object *> objects;

	/**
	 * @brief Vector of Object references to all objects in the current timestep
	 * of this game instance.
	 * 
	 * The objects vector is indexed by each Object's global id; that is, the 
	 * Object reference at index x is to the Object with glboal id x.
	 */
	//std::vector<Object&> objects;

	/**
	 * @brief Global ID freelist, used for assigning global object IDs.
	 */
	//std::queue<unsigned int> global_ID_freelist;

	//	TODO: Add type-specific object arrays (e.g., vector<Player> players,
	//	vector<Wall> walls, etc.)

	SmartVector<Object> base_objects;

	/**
	 * @brief Vector of all Object instances whose actual type is closest to
	 * Object.
	 */
	//std::vector<Object> base_objects;

	/**
	 * @brief Type-specific ID freelist for the base_objects array.
	 */
	//std::queue<unsigned int> base_objects_ID_freelist;

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