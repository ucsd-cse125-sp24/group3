#pragma once
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/glm.hpp>
#include <glm/gtx/string_cast.hpp>
#include <vector>

/* ServerGameState Constants */
#define MAX_ENEMY_VALUE			500
#define NUM_PLAYERS 4

/*	Maze Constants */
#define	MAX_MAZE_COLUMNS		10000
#define	MAZE_CEILING_HEIGHT		10	// Maze ceiling height in meters

/*	GridCell Constants	*/
#define DEFAULT_GRIDCELL_WIDTH	3
#define DM_Z_DISCOUNT 0.2
#define GRIDS_PER_FLOOR_OBJECT 5

// Player Stat Constants
#define INITIAL_HEALTH 100

/*	Inventory	*/
#define	INVENTORY_SIZE			4

/*	Spell Constants	*/
#define	FIRE_LIMIT				50
#define	HEAL_LIMIT				3
#define	TELEPORT_LIMIT			1
#define	TELEPORT_RANGE			15

/*	Potion Stats	*/
#define	RESTORE_HEALTH			20
#define HEALTH_DURATION			0

#define	NAUSEA_SCALAR			-1
#define NAUSEA_DURATION			10

#define INVIS_DURATION			15

#define	INVINCIBLITY_SCALAR		100
#define INVINCIBLITY_DUR		15

/*	Weapon Stats	*/
#define	SWORD_DMG				5
#define DAGGER_DMG				7
#define HAMMER_DMG				15
#define	LIGHTNING_DMG			99

#define	SWORD_PREP				200
#define	SWORD_DUR				300
#define	SWORD_TOTAL				500

#define	DAGGER_PREP				150
#define	DAGGER_DUR				150
#define	DAGGER_TOTAL			300

#define	HAMMER_PREP				500
#define	HAMMER_DUR				350
#define	HAMMER_TOTAL			850

#define	LIGHTNING_PREP			1000
#define	LIGHTNING_DUR		    400

#define	DM_MANA_TOTAL			30
#define	DM_MANA_REGEN			1
#define	LIGHTNING_MANA			10
#define	LIGHT_CUT_MANA			5

/*	Mirror Item	*/
//	Mirror use duration in seconds
#define	MIRROR_USE_DURATION		30

/*	Game	*/
#define	GRAVITY					0.03f
#define	PLAYER_SPEED 			1.65f
#define JUMP_SPEED				0.59f

/* DM Constants */
#define MAX_TRAPS 10
#define TRAP_INVENTORY_SIZE 7
#define TRAP_TIME 10
#define TRAP_COOL_DOWN 5
#define ITEM_SPAWN_PROB	0.1
#define ITEM_SPAWN_BOUND 3
#define LIGHTNING_LIGHT_CUT_TICKS 100
#define LIGHT_CUT_TICKS 200
#define LIGHT_CUT_RANGE 25.0