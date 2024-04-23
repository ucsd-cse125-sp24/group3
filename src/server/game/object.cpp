#include "server/game/object.hpp"
#include "server/game/constants.hpp"
#include "shared/game/sharedobject.hpp"

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
	shared.facing = this->physics.facing;
	shared.position = this->physics.position;

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
	representation += tabs + "\ttype id:\t\t" + std::to_string(this->typeID) + '\n';
	representation += tabs + "\tObjectType:\t\t" + objectTypeString(this->type) + '\n';
	representation += tabs + "\tPhysics:\t\t" + '\n';
	representation += this->physics.to_string(tab_offset + 1) + '\n';
	representation += tabs + "}";

	return representation;
}

std::string objectTypeString(ObjectType type) {
	switch (type) {
		case ObjectType::Object:
			return "Object";
		default:
			return "Unknown";
	}
}

std::string Physics::to_string(unsigned int tab_offset) {
	//	Return a string representation of this Physics struct

	std::string tabs;

	for (unsigned int i = 0; i < tab_offset; i++)
		tabs += '\t';

	std::string representation = tabs + "{\n";
	representation += tabs + "\tmovable:\t\t" + (this->movable ? "true" : "false") + '\n';
	representation += tabs + "\tposition:\t\t" + glm::to_string(this->position) + '\n';
	representation += tabs + "\tvelocity:\t\t" + glm::to_string(this->velocity) + '\n';
	representation += tabs + "\tacceleration:\t\t" + glm::to_string(this->acceleration) + '\n';
	representation += tabs + "\tfacing:\t\t\t" + glm::to_string(this->facing) + '\n';
	representation += tabs + "}";

	return representation;
}