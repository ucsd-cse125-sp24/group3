#pragma once

/*	Constants	*/
#define	FIRST_TIMESTEP			0
#define TIMESTEP_LEN			std::chrono::milliseconds(30)
#define	MAX_PLAYERS				4

/*	Game phase information	*/
//	Time limit initially set to 5 minutes
#define	TIME_LIMIT_MS				std::chrono::milliseconds(300000)
//#define	TIME_LIMIT_MS			std::chrono::milliseconds(5 * 60 * 1000)

/*	Number of player deaths to update match state to MatchState::RelayRace	*/
#define	PLAYER_DEATHS_TO_RELAY_RACE	5
//#define	PLAYER_DEATHS_TO_RELAY_RACE	15

/* Default model sizes */
#define BEAR_DIMENSIONS         glm::vec3(14.163582, 17.914591, 10.655818)
#define FIRE_PLAYER_DIMENSIONS  glm::vec3(8.008834, 10.069769, 2.198592)
#define SUNGOD_DIMENSIONS       glm::vec3(3.281404, 9.543382, 7.974873) 
#define ARROW_DIMENSIONS        glm::vec3(45.025101, 68.662003, 815.164001)
#define ARROW_TRAP_DIMENSIONS   glm::vec3(2.815553, 3.673665, 1.588817)
