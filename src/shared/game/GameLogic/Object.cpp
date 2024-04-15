#include "shared/game/GameLogic/Object.hpp"

//	Initialize static object counter
unsigned int Object::numObjects = 0;

Object::Object() {
	//	Assign a new unique id (not accounting for overflow)
	this->id = Object::numObjects++;
}

std::string Object::to_string(unsigned int tab_offset) {
	//	Return a string representation of this object
	std::string tabs;

	for (unsigned int i = 0; i < tab_offset; i++)
		tabs += '\t';

	std::string representation = tabs + "{\n";
	representation += tabs + "\tid:\t\t" + std::to_string(this->id) + '\n';
	representation += tabs + "\tposition:\t\t" + this->position.to_string() + '\n';
	representation += tabs + "\tvelocity:\t\t" + this->velocity.to_string() + '\n';
	representation += tabs + "\tacceleration:\t\t" + this->acceleration.to_string() + '\n';
	representation += tabs + "}";

	return representation;
}

Object::~Object() {


}