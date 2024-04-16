#include "shared/game/gamestate.hpp"
#include <string>

GameState::GameState(unsigned int timestep, unsigned int timestep_length) {
	this->timestep = timestep;
	this->timestep_length = timestep_length;
}

void GameState::update() {
	//	Iterate through all the objects in the GameState and update their
	//	positions and velocities.
	for (Object& o : this->objects) {
		//	o.position [meters] 
		//	= o.position [meters] + (o.velocity [meters / timestep] * 1 timestep)
		o.position = o.position + o.velocity;

		//	o.velocity [meters / timestep]
		//	= o.velocity [meters / timestep]
		//		+ (o.acceleration [meters / timestep^2] * 1 timestep)
		o.velocity = o.velocity + o.acceleration;
	}

	//	Increment timestep
	this->timestep++;
}

Object* GameState::createObject() {
	//	Create a new object and return a pointer to it
	this->objects.push_back(Object());

	//	Return a pointer to the newly created object
	return &(this->objects.back());
}

bool GameState::removeObject(unsigned int id) {
	//	Note: this is NOT a performant implementation and will not scale well.
	//	This implementation is only for simplicity (to get the initial prototype
	//	working) and will need to be optimized when the number of objects
	//	increases.

	//	Iterate through the objects vector and search for an object with the
	//	given id.
	for (int i = 0; i < this->objects.size(); i++) {
		Object& current = this->objects.at(i);

		if (current.id == id)
		{
			//	Found the object with the given id; remove it from the objects
			//	vector
			this->objects.erase(this->objects.begin() + i);

			return true;
		}
	}

	//	Didn't find an object with the given id
	return false;
}

Object* GameState::getObject(unsigned int id) {
	//	Note: this is NOT a performant implementation and will not scale well.

	//	Iterate through the objects vector and search for an object with the
	//	given id.
	for (int i = 0; i < this->objects.size(); i++) {
		Object& current = this->objects.at(i);

		if (current.id == id) {
			//	Found the object with the given id
			return &current;
		}
	}

	//	Didn't find an object with the given id
	return nullptr;
}

std::string GameState::to_string() {
	std::string representation = "{";
	representation += "\n\ttimestep:\t\t" + std::to_string(this->timestep);
	representation += "\n\ttimestep len:\t\t" + std::to_string(this->timestep_length);
	representation += "\n\tobjects: [\n";

	for (int i = 0; i < this->objects.size(); i++) {
		representation += this->objects.at(i).to_string(2);

		if (i < this->objects.size() - 1) {
			representation += ",";
		}

		representation += "\n";
	}

	representation += "\t]\n}";

	return representation;
}

