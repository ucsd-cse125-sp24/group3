#pragma once

/*	Constants	*/
#define	FIRST_TIMESTEP			0
#define TIMESTEP_LEN			std::chrono::milliseconds(30)
#define	MAX_PLAYERS				4

/*	Game phase information	*/
//	Time limit initially set to 5 minutes
#define	TIME_LIMIT_S std::chrono::seconds(300)

/* Default model sizes */
#define BEAR_DIMENSIONS         glm::vec3(14.163582, 17.914591, 10.655818)
#define FIRE_PLAYER_DIMENSIONS      glm::vec3(4.0f, 10.069769, 4.0f)
#define LIGHTNING_PLAYER_DIMENSIONS glm::vec3(4.0f, 10.069769, 4.0f)
#define WATER_PLAYER_DIMENSIONS     glm::vec3(4.0f, 10.069769, 4.0f)
#define SUNGOD_DIMENSIONS       glm::vec3(3.281404, 9.543382, 7.974873) 
#define ARROW_DIMENSIONS        glm::vec3(25.025101, 68.662003, 333.333333)
#define ARROW_TRAP_DIMENSIONS   glm::vec3(2.815553, 3.673665, 1.588817)
#define FLOOR_SPIKE_DIMENSIONS  glm::vec3(3.0f, 1.0f, 3.0f)
#define LAVA_DIMENSIONS         glm::vec3(3.0f, 0.01f, 3.0f)

#define PLAYER_BBOX_SCALE           0.35f
#define PLAYER_MODEL_SCALE          0.004f

/*	Number of player deaths to update match state to MatchState::RelayRace	*/
#define	PLAYER_DEATHS_TO_RELAY_RACE	3
