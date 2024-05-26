#include "shared/game/sharedgamestate.hpp"
#include "shared/game/event.hpp"

void SharedGameState::update(const SharedGameState& other) {
    // copy over static data that is sent every update
    this->lobby = other.lobby;

    //  DEBUG
    /*
    std::cout << "SharedGameState::update()" << std::endl;
    std::cout << other.lobby.to_string() << std::endl;
    */

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

/*  Debug Methods   */

std::string LobbyPlayer::to_string(unsigned int tab_offset) {
    //  Return a string representation of this LobbyPlayer struct

    std::string tabs;

    for (unsigned int i = 0; i < tab_offset; i++)
        tabs += '\t';

    std::string representation = tabs + "{\n";
    representation += tabs + "\tglobal id:\t\t" + std::to_string(this->id) + '\n';

    std::string role;

    switch (this->desired_role) {
        case PlayerRole::Player:
            role = "Player";
            break;
        case PlayerRole::DungeonMaster:
            role = "Dungeon Master";
            break;
        case PlayerRole::Unknown:
            role = "Unknown";
            break;
    }

    representation += tabs + "\tdesired role:\t\t" + role + '\n';
    representation += tabs + "\tready:\t\t" + (this->ready ? "true" : "false") + '\n';
    representation += tabs + "}";

    return representation;
}

std::string Lobby::to_string(unsigned int tab_offset) const {
    //  Return a string representation of this Lobby struct

    std::string tabs;   

    for (unsigned int i = 0; i < tab_offset; i++)
        tabs += '\t';

    std::string representation = tabs + "{\n";
    representation += tabs + "\tname:\t\t" + this->name + '\n';
    representation += tabs + "\tplayers: [\n";

    for (int i = 0; i < this->max_players; i++) {
        boost::optional<LobbyPlayer> player = this->players[i];

        if (!player.has_value()) {
            representation += tabs + "\t\tEmpty\n";
        }
        else {
            representation += player.get().to_string(tab_offset + 2) + '\n';
        }
    }

    representation += tabs + "\t]\n";

    representation += tabs + "\tmax players:\t\t" + std::to_string(max_players) + '\n';
    representation += tabs + "}";

    return representation;
}