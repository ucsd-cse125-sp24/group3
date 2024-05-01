#pragma once
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/glm.hpp>
#include <glm/gtx/string_cast.hpp>
#include <vector>

#define NUM_PLAYERS 4
#define MAX_ENTITIES 1000
#define MAX_COMPONENTS 32
#define MAX_WALLS 1000
#define MAX_TRAPS 10
#define MAX_SPELLS 4
#define MAX_ITEMS 4

/*	Constants	*/
#define	FIRST_TIMESTEP			0
#define TIMESTEP_LEN			std::chrono::milliseconds(30)
#define	MAX_PLAYERS				4

//	SmartVector capacities
#define	MAX_NUM_OBJECTS			100
#define MAX_NUM_BASE_OBJECTS	100

// Player Stat Constants
#define INITIAL_HEALTH 100

/*	Game	*/
#define	GRAVITY					0.03f
#define	PLAYER_SPEED 			0.5f