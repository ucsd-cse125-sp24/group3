#include "shared/game/sharedgamestate.hpp"

void SharedGameState::update(const SharedGameState& other) {
    // copy over static data that is sent every update
    this->lobby = other.lobby;
    this->phase = other.phase;
    this->timestep = other.timestep;
    this->matchPhase = other.matchPhase;
    this->timesteps_left = other.timesteps_left;
    this->playerVictory = other.playerVictory;
    this->numPlayerDeaths = other.numPlayerDeaths;

    // Update our own objects hash map based on new updates
    for (const auto& [id, updated_obj] : other.objects) { // cppcheck-suppress unassignedVariable
        this->objects[id] = updated_obj;
    }
}

const boost::optional<LobbyPlayer>& Lobby::getPlayer(int playerIndex) const {
    return this->players[playerIndex - 1];
}

const boost::optional<LobbyPlayer>& Lobby::getPlayer(EntityID id) const {
    for (auto& player : this->players) {
        if (player.has_value() && player.get().id == id)
            return player;
    }

    return boost::none;
}

int Lobby::numPlayersInLobby() const {
    int numPlayers = 0;
    for (auto& player : this->players) {
        if (player.has_value()) {
            numPlayers++;
        }
    }

    return numPlayers;
}