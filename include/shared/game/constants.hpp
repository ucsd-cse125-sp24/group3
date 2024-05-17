#pragma once

/*	Constants	*/
#define	FIRST_TIMESTEP			0
#define TIMESTEP_LEN			std::chrono::milliseconds(30)
#define	MAX_PLAYERS				4

/*	Game phase information	*/
//	Time limit initially set to 5 minutes
//#define	TIME_LIMIT				std::chrono::milliseconds(5 * 60 * 1000)
//#define TIME_LIMIT				std::chrono::milliseconds(10000)
#define	TIME_LIMIT_MS			std::chrono::milliseconds(5 * 60 * 1000)
