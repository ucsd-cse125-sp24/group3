#pragma once

#include "server/game/servergamestate.hpp"
#include "shared/game/sharedobject.hpp"
#include "shared/utilities/smartvector.hpp"
#include "shared/utilities/serialize_macro.hpp"

#include <chrono>

/**
 * @brief The SharedGameState is a representation of the ServerGameState
 * instance maintained by the server and is generated by the server in every
 * timestep. It is intended only for use by the client(s).
 */
struct SharedGameState {
	SmartVector<SharedObject> objects;

	std::chrono::milliseconds timestep_length;

	unsigned int timestep;

	Lobby lobby;

	GamePhase phase;

	SharedGameState() {
		this->phase = GamePhase::TITLE_SCREEN;
		this->timestep = FIRST_TIMESTEP;
		this->timestep_length = TIMESTEP_LEN;
		this->lobby.max_players = MAX_PLAYERS;

		//	Initialize SmartVectors with max sizes
		this->objects = SmartVector<SharedObject>(MAX_NUM_OBJECTS);
	}

	SharedGameState(GamePhase start_phase, GameConfig config) {
		this->phase = start_phase;
		this->timestep = FIRST_TIMESTEP;
		this->timestep_length = config.game.timestep_length_ms;
		this->lobby.max_players = config.server.max_players;

		//	Initialize SmartVectors with max sizes
		this->objects = SmartVector<SharedObject>(MAX_NUM_OBJECTS);
	}

	DEF_SERIALIZE(Archive& ar, const unsigned int version) {
		ar & phase& lobby.max_players & lobby.players & objects;
	}
};