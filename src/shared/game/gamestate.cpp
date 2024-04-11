#include "shared/game/gamestate.hpp"

#include <string>

/**
 *	GameState constructor.
 */
GameState::GameState() {
	//	Initialize time_step length to 30 ms
	this->time_step_len = 30;

	//	Initial time_step is 0
	this->time_step = 0;
}

/**
 * Update the current game state from the previous tick to the next tick.
 */
void GameState::updateState() {
	//	Update GameState from previous tick to the current one

	//	Handle all events that arrived / trigger during this tick

	//	Update all object positions
}

void GameState::createObject(Object * object) {
	//	Create a new object and put it in the world (add to objects
	//	vector)
	this->objects.push_back(*object);
}

//	Fake main() for testing - pretend to be the server
int main() {
	//	Create a GameState object
	GameState * state = new GameState();

	//	Create a new object in the game world
	Object* obj1 = new Object();


	state->createObject(obj1);


	//	Every time step, run the update function
	while (1) {
		//	Call update function
		state->updateState();
	}
}

