#include "server/game/object.hpp"
#include "server/game/constants.hpp"
#include "shared/game/sharedobject.hpp"

/*	Constructors and Destructors	*/

Object::Object(ObjectType type) {
	//	Set object type to Object
	this->type = type;
	
	//	Initialize object Physics
	//	By default, the newly created object spawns at the origin without any
	//	velocity or acceleration, and is movable. The object faces toward the
	//	x-axis.
	this->physics.shared.corner = glm::vec3(0.0f, 0.0f, 0.0f);
	this->physics.shared.facing = glm::vec3(1.0f, 0.0f, 0.0f);

	this->physics.velocity = glm::vec3(0.0f, 0.0f, 0.0f);
	this->physics.velocityMultiplier = glm::vec3(1.0f, 1.0f, 1.0f);
	this->physics.movable = true;

	//	By default, the object is not assigned a collider (must be explicitly
	//	assigned) (TODO: Could change this by assigning a default collider?)
	this->physics.boundary = nullptr;
}

Object::~Object() {}

/*	Static properties	*/
std::unordered_map<ModelType, glm::vec3> Object::models ({
	{ModelType::Cube, glm::vec3(1.0, 1.0, 1.0) },
	//	TODO: Update these to the correct dimensions!!
	{ModelType::Player, glm::vec3(0)},
	{ModelType::WarrenBear, glm::vec3(0)}
});

/*	SharedGameState generation	*/
SharedObject Object::toShared() {
	SharedObject shared;

	shared.globalID = this->globalID;
	shared.type = this->type;
	shared.physics = this->physics.shared;
	shared.model = this->model;

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

std::string Physics::to_string(unsigned int tab_offset) {
	//	Return a string representation of this Physics struct

	std::string tabs;

	for (unsigned int i = 0; i < tab_offset; i++)
		tabs += '\t';

	std::string representation = tabs + "{\n";
	representation += tabs + "\tmovable:\t\t" + (this->movable ? "true" : "false") + '\n';
	representation += tabs + "\tvelocity:\t\t" + glm::to_string(this->velocity) + '\n';
	representation += tabs + "\velocityMultiplier:\t\t" + glm::to_string(this->velocityMultiplier) + '\n';
	representation += tabs + "\tfacing:\t\t\t" + glm::to_string(this->shared.facing) + '\n';
	representation += tabs + "\tdimensions:\t\t\t" + glm::to_string(this->shared.dimensions) + '\n';
	representation += tabs + "}";

	return representation;
}