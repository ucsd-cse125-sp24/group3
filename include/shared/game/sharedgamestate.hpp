#pragma once

#include <memory>
#include <string>
#include <chrono>
#include <vector>
#include <boost/serialization/string.hpp>

//#include "server/game/servergamestate.hpp"
#include "shared/game/sharedobject.hpp"
#include "shared/utilities/smartvector.hpp"
#include "shared/utilities/serialize_macro.hpp"
#include "shared/utilities/config.hpp"
//#include "server/game/constants.hpp"
#include "shared/game/constants.hpp"

//	Forward declaration of PlayerRole enum to avoid circular reference
enum class PlayerRole;

enum class GamePhase {
	TITLE_SCREEN,
	LOBBY,
	GAME,
	RESULTS
};

enum class MatchPhase {
	MazeExploration,
	RelayRace
};

/**
 * @brief Information about a player that has connected to the
 * current lobby.
 */
struct LobbyPlayer {
	/**
	 * @brief Player's EntityID (this is the EntityID of their Player
	 * or DungeonMaster object in the game state)
	 */
	EntityID id;

	/**
	 * @brief This is the player's desired role (though they may not
	 * actually play as this role - the server must settle ties if
	 * multiple (or no) players want to play as the Dungeon Master 
	 * for instance)
	 */
	PlayerRole desired_role;

	/**
	 * @brief Whether the player is in the Ready state on the lobby
	 * screen.
	 */
	bool ready;

	LobbyPlayer() {}

	LobbyPlayer(EntityID id, PlayerRole desired_role, bool ready)
		: id(id), desired_role(desired_role), ready(ready) {}

	DEF_SERIALIZE(Archive& ar, unsigned int version) {
		ar& id& desired_role & ready;
	}

	/*	Debug Methods	*/
	
	/**
	 * @brief Generates a string representation of this Lobby Player struct.
	 * @return std::string representation of this Lobby Player struct.
	*/
	std::string to_string(unsigned int tab_offset);
	std::string to_string() { return this->to_string(0); }
};

/**
 * @brief Information about the current lobby of players.
 */
struct Lobby {
	/**
	 * @brief name of the lobby as set by the server
	 */
	std::string name;

	/**
	 * @brief A vector of length max_players that maps a player's index
	 * (technically, index - 1 as player indices are 1-indexed) to their
	 * LobbyPlayer info struct (though since a player index could at
	 * present not be assigned, this maps to a boost::optional<LobbyPlayer>)
	 */
	std::vector<boost::optional<LobbyPlayer>> players;

	/**
	 * @brief The maximum number of players that this game instance can support.
	 */
	int max_players;

	/**
	 * @brief Default Lobby constructor supports exactly 4 players
	 */
	Lobby() : Lobby(4) {}

	explicit Lobby(int max_players) : Lobby("Default Lobby Name", max_players) {}

	Lobby(const std::string& name, int max_players) : name(name), max_players(max_players) {
		//	Reserve max_players slots in the players vector
		for (int i = 0; i < max_players; i++) {
			this->players.push_back(boost::none);
		}
	}

	/**
	 * @brief Given a player's 1-indexed player index, this method returns 
	 * a reference to the boost::optional<LobbyPlayer> that will contain the
	 * LobbyPlayer struct for that player if that player index has been assigned.
	 * @param playerIndex 1-indexed player index of the player whose LobbyPlayer
	 * struct is requested. (I.e., Player 1's 1-indexed player index is 1 though it
	 * will be stored at index 0 in the players vector)
	 * @return Reference to the boost::optional<LobbyPlayer> that contains the LobbyPlayer
	 * struct of the player with the requested 1-indexed player index. If that player
	 * index has not been assigned, the boost::optional will be empty.
	*/
	const boost::optional<LobbyPlayer>& getPlayer(int playerIndex) const;

	/**
	 * @brief Returns a player's LobbyPlayer struct by the player's EntityID.
	 * @param id EntityID of the player whose LobbyPlayer struct is requested.
	 * @return Reference to the boost::optional<LobbyPlayer> that contains the LobbyPlayer
	 * struct of the player with the given EntityID; if no player in the lobby has this
	 * EntityID, the boost::optional will be empty.
	*/
	const boost::optional<LobbyPlayer>& getPlayer(EntityID id) const;

	/**
	 * @brief Returns the number of players in the lobby
	 * Note: use this method and note players.size() to determine the number of
	 * current players in the lobby! players.size() should always equal
	 * max_players
	 * @return Number of current players in the lobby
	*/
	int numPlayersInLobby() const;


	DEF_SERIALIZE(Archive& ar, unsigned int version) {
		ar & name & players & max_players;
	}

	//	TODO: Add a player role listing? I.e., which player is playing which
	//	character and which player is playing as the Dungeon Master?

	/*	Debug Methods	*/

	/**
	 * @brief Generates a string representation of this Lobby struct.
	 * @return std::string representation of this Lobby struct
	 */
	std::string to_string(unsigned int tab_offset) const;
	std::string to_string() const { return this->to_string(0); }
};

/**
 * @brief The SharedGameState is a representation of the ServerGameState
 * instance maintained by the server and is generated by the server in every
 * timestep. It is intended only for use by the client(s).
 */
struct SharedGameState {
	std::unordered_map<EntityID, boost::optional<SharedObject>> objects;

	unsigned int timestep;

	Lobby lobby;

	GamePhase phase;

	MatchPhase matchPhase;

	unsigned int timesteps_left;

	bool playerVictory;

	unsigned int numPlayerDeaths;

	std::vector<std::pair<EntityID, glm::ivec2>> player_grid_positions;

	SharedGameState():
		objects(std::unordered_map<EntityID, boost::optional<SharedObject>>()),
		player_grid_positions()
	{
		this->phase = GamePhase::TITLE_SCREEN;
		this->timestep = FIRST_TIMESTEP;
		this->lobby.max_players = MAX_PLAYERS;
		this->matchPhase = MatchPhase::MazeExploration;
		this->timesteps_left = TIME_LIMIT_MS / TIMESTEP_LEN;
		this->playerVictory = false;
		this->numPlayerDeaths = 0;
	}

	SharedGameState(GamePhase start_phase, const GameConfig& config):
		objects(std::unordered_map<EntityID, boost::optional<SharedObject>>()),
		player_grid_positions()
	{
		this->phase = start_phase;
		this->timestep = FIRST_TIMESTEP;
		this->lobby.max_players = config.server.max_players;
		this->lobby.name = config.server.lobby_name;
		this->matchPhase = MatchPhase::MazeExploration;
		this->timesteps_left = TIME_LIMIT_MS / TIMESTEP_LEN;
		this->playerVictory = false;
		this->numPlayerDeaths = 0;
	}

	DEF_SERIALIZE(Archive& ar, const unsigned int version) {
		ar & objects & timestep & lobby & phase & matchPhase
			& timesteps_left & playerVictory & numPlayerDeaths
			& player_grid_positions;
	}

	/**
	 * Updates this SharedGameState with the changes from the incoming SharedGameState
	 * 
	 * @param update New Partial gamestate update from the server
	 */
	void update(const SharedGameState& update);
};