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

/*	Constants	*/
#define	FIRST_TIMESTEP			0
#define TIMESTEP_LEN			std::chrono::milliseconds(30)
#define	MAX_PLAYERS				4

/*	Maze Constants */
#define	MAX_MAZE_COLUMNS		10000
#define	MAZE_CEILING_HEIGHT		10	// Maze ceiling height in meters

/*	GridCell Constants	*/
#define DEFAULT_GRIDCELL_WIDTH	3
#define DM_Z_DISCOUNT 0.05

// Player Stat Constants
#define INITIAL_HEALTH 100

/*	Inventory	*/
#define	INVENTORY_SIZE			4

/*	Spell Constants	*/
#define	CAST_LIMIT				5

/*	Potion Stats	*/
#define	RESTORE_HEALTH			20
#define HEALTH_DURATION			0

#define	NAUSEA_SCALAR			-1.0f
#define NAUSEA_DURATION			10

#define INVIS_DURATION			15

#define	INVINCIBLITY_SCALAR		100
#define INVINCIBLITY_DUR		15

/*	Game	*/
#define	GRAVITY					0.03f
#define	PLAYER_SPEED 			1.5f
#define JUMP_SPEED				0.5f

/* Default model sizes */
#define BEAR_DIMENSIONS         glm::vec3(14.163582, 17.914591, 10.655818)
#define FIRE_PLAYER_DIMENSIONS  glm::vec3(8.008834, 10.069769, 2.198592)

/* DM Constants */
#define MAX_TRAPS 10
#define TRAP_INVENTORY_SIZE 7
#define TRAP_TIME 10