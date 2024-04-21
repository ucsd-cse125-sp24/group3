#include "shared/game/gamelogic/object.hpp"
#include "shared/game/gamelogic/constants.hpp"
#include "shared/game/sharedgamestate/sharedobject.hpp"

/*	Constructors and Destructors	*/

Object::Object() {
	//	Set object type to Object
	this->type = ObjectType::Object;
	
	//	Initialize object Physics
	//	By default, the newly created object spawns at the origin without any
	//	velocity or acceleration, and is movable. The object faces toward the
	//	x-axis.
	this->physics.position = glm::vec3(0.0f, 0.0f, 0.0f);
	this->physics.velocity = glm::vec3(0.0f, 0.0f, 0.0f);
	this->physics.acceleration = glm::vec3(0.0f, 0.0f, 0.0f);
	this->physics.facing = glm::vec3(1.0f, 0.0f, 0.0f);
	this->physics.movable = true;

	//	By default, the object is not assigned a collider (must be explicitly
	//	assigned) (TODO: Could change this by assigning a default collider?)
	this->physics.boundary = nullptr;
}

Object::~Object() {}

/*	SharedGameState generation	*/
SharedObject Object::generateSharedObject() {
	SharedObject shared;

	shared.globalID = this->globalID;
	shared.typeID = this->typeID;
	shared.type = this->type;
	shared.physics = this->physics.generateSharedPhysics();

	return shared;
}

/*	Debugger Methods	*/

std::string Object::to_string(unsigned int tab_offset) {
	//	Return a string representation of this object
	std::string tabs;

	for (unsigned int i = 0; i < tab_offset; i++)
		tabs += '\t';

	std::string representation = tabs + "{\n";
	representation += tabs + "\tglobal id:\t\t" + std::to_string(this->globalID) + '\n';
	representation += tabs + "\type id:\t\t" + std::to_string(this->typeID) + '\n';
	representation += tabs + "\tposition:\t\t" + glm::to_string(this->physics.position) + '\n';
	representation += tabs + "\tvelocity:\t\t" + glm::to_string(this->physics.velocity) + '\n';
	representation += tabs + "\tacceleration:\t\t" + glm::to_string(this->physics.acceleration) + '\n';
	representation += tabs + "\tfacing:\t\t" + glm::to_string(this->physics.facing) + '\n';
	representation += tabs + "}";

	return representation;
}



SharedPhysics Physics::generateSharedPhysics() {
	SharedPhysics shared;

	shared.facing = this->facing;
	shared.position = this->position;

	return shared;
}