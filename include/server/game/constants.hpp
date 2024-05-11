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

/*	Maze Constants */
#define	MAX_MAZE_COLUMNS		1024
#define	MAZE_CEILING_HEIGHT		10	// Maze ceiling height in meters

/*	GridCell Constants	*/
#define DEFAULT_GRIDCELL_WIDTH	3

// Player Stat Constants
#define INITIAL_HEALTH 100

/*	Potion Stats	*/
#define	RESTORE_HEALTH			20
#define HEALTH_DURATION			0

#define	SPEED_BOOST 			1.2f
#define SPEED_DURATION			15

#define INVIS_DURATION			15

/*	Game	*/
#define	GRAVITY					0.03f
#define	PLAYER_SPEED 			1.5f
#define JUMP_SPEED				0.5f