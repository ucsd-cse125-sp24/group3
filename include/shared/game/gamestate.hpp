#pragma once

#include "shared/utilities/serialize_macro.hpp"
#include "shared/game/gamelogic/object.hpp"
#include "shared/utilities/typedefs.hpp"
#include "shared/utilities/config.hpp"

#include <string>
#include <vector>
#include <chrono>
#include <unordered_map>

enum class GamePhase {
    TITLE_SCREEN,
    LOBBY,
    GAME
};

//	Constants
#define	FIRST_TIMESTEP	0
#define	TIMESTEP_LEN	30

class GameState {
public:
    /**
     * @brief GameState constructor that sets data members based on the config
     *
     * @param phase starting phase of the game
     * @param config Config options
     */
    GameState(GamePhase phase, GameConfig config);

    /**
     * @brief Updates this GameState instance from the current timestep to the
     * next one.
     */
    void update();

    /**
     * @brief Creates a new Object and places it in the world (adds it to the
     * objects vector). The object's position is at the origin and has 0 vectors
     * for velocity and acceleration.
     *
     * @return a pointer to the newly added object.
     */
    Object* createObject();

    /**
     * @brief Attempt to remove an object with the given id.
     * @param id ID of the object to remove from this game instance.
     * @return true if successfully removed the object and false otherwise.
     */
    bool removeObject(unsigned int id);

    /**
     * @brief Attempts to retrieve the object with the given id.
     * @param id ID of the object to retrieve
     * @return a pointer to the object with the given id, or nullptr if none
     * exists.
     */
    Object* getObject(unsigned int id);

    std::string to_string();
    unsigned int getTimestep() { return this->timestep; }
    std::chrono::milliseconds getTimestepLength() { return this->timestep_length; }

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

    DEF_SERIALIZE(Archive& ar, const unsigned int version) {
        ar & phase & lobby.max_players & lobby.players;
    }

private:
    /**
     *  Vector of all objects in the current timestep of this game instance.
     */
    std::vector<Object> objects;

    /**
     *  Timestep length in milliseconds.
     */
    std::chrono::milliseconds timestep_length;

    /**
     *  Current timestep (starts at 0)
     */
    unsigned int timestep;

    /**
     * Current phase of the game
     */
    GamePhase phase;

    /**
     * Information about the lobby
     */
    struct {
        std::unordered_map<EntityID, std::string> players;
        int max_players;
    } lobby;
};
