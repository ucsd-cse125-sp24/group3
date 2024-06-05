#pragma once

/*	Constants	*/
#define	FIRST_TIMESTEP			0
#define TIMESTEP_LEN			std::chrono::milliseconds(30)
#define	MAX_PLAYERS				4

/*	Game phase information	*/
//	Time limit initially set to 5 minutes
#define	TIME_LIMIT_S std::chrono::seconds(300)

/*	Number of player deaths to update match state to MatchState::RelayRace	*/
#define	PLAYER_DEATHS_TO_RELAY_RACE	3