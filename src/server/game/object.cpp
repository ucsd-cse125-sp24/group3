#include "server/game/object.hpp"
#include "server/game/constants.hpp"
#include "shared/game/sharedobject.hpp"

/*	Constructors and Destructors	*/

Object::Object(ObjectType type, Physics physics, ModelType modelType):
	physics(physics)
{
	//	Set object type to Object
	this->type = type;
	this->setModel(modelType);
}

Object::~Object() {}

void Object::setModel(ModelType type) {
	//	Set model information
	this->modelType = type;

	// Right now all of our cube's dimensions are being set manually
	// so we really only want to do this change if this is an actual model
	// with preset dimensions
	// Maybe we need to think of a better way to do this later on
	if (type != ModelType::Cube) {
		this->physics.shared.dimensions = Object::models.find(this->modelType)->second;
	}

}

/*	Static properties	*/
std::unordered_map<ModelType, glm::vec3> Object::models ({
	{ModelType::Cube, glm::vec3(1.0, 1.0, 1.0) },
    // NOTE: making the player dimensions pretty small since 
    // otherwise it collides with the maze passages and you  
    // can't move around anywhere. we should eventually solve this
    // by tucking in the player's arms since right now they're 
    // spread out in the model
	{ModelType::Player, (FIRE_PLAYER_DIMENSIONS / 4.0f)},
	{ModelType::WarrenBear, (BEAR_DIMENSIONS / 4.0f)}
});

/*	SharedGameState generation	*/
SharedObject Object::toShared() {
	SharedObject shared;

	shared.globalID = this->globalID;
	shared.type = this->type;
	shared.physics = this->physics.shared;
	shared.modelType = this->modelType;

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