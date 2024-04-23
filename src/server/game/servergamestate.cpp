#include "server/game/servergamestate.hpp"
#include "shared/game/sharedgamestate.hpp"

/*	Constructors and Destructors	*/

ServerGameState::ServerGameState(GamePhase start_phase, GameConfig config) {
	this->phase = start_phase;
	this->timestep = FIRST_TIMESTEP;
	this->timestep_length = config.game.timestep_length_ms;
	this->lobby.max_players = config.server.max_players;

	//	Initialize SmartVectors with max sizes
	this->objects = SmartVector<Object*>(MAX_NUM_OBJECTS);

	this->base_objects = SmartVector<Object>(MAX_NUM_BASE_OBJECTS);
}

ServerGameState::ServerGameState(GamePhase start_phase) {
	this->phase = start_phase;
	this->timestep = FIRST_TIMESTEP;
	this->timestep_length = TIMESTEP_LEN;
	this->lobby.max_players = MAX_PLAYERS;

	//	Initialize SmartVectors with max sizes
	this->objects = SmartVector<Object*>(MAX_NUM_OBJECTS);

	this->base_objects = SmartVector<Object>(MAX_NUM_BASE_OBJECTS);
}

ServerGameState::ServerGameState() : ServerGameState(GamePhase::LOBBY) {}

ServerGameState::~ServerGameState() {}

/*	SharedGameState generation	*/
SharedGameState ServerGameState::generateSharedGameState() {
	//	Create a new SharedGameState instance and populate it with this
	//	ServerGameState's data
	SharedGameState shared;

	//	Initialize object vector
	for (int i = 0; i < this->objects.size(); i++) {
		Object** ptrToPtr = this->objects.get(i);

		if (ptrToPtr == nullptr) {
			//	Push empty object to SharedObject SmartVector
			shared.objects.pushEmpty();
		}
		else {
			Object* object = *ptrToPtr;
			
			//	Create a SharedObject from this object
			shared.objects.push(object->generateSharedObject());
		}
	}

	//	Copy timestep data
	shared.timestep = this->timestep;
	shared.timestep_length = this->timestep_length;

	//	Copy Lobby data
	shared.lobby = this->lobby;
	
	//	Copy GamePhase
	shared.phase = this->phase;

	return shared;
}

/*	Update methods	*/

void ServerGameState::update() {
	//	TODO: fill update() method with updating object movement
	updateMovement();

	//	Increment timestep
	this->timestep++;
}

void ServerGameState::updateMovement() {
	//	Update all movable objects' movement

	//	Iterate through all objects in the ServerGameState and update their
	//	positions and velocities if they are movable.

	Object** ptrToPtr;
	Object* object;
	for (int i = 0; i < this->objects.size(); i++) {
		ptrToPtr = this->objects.get(i);

		if (ptrToPtr == nullptr)
			continue;
		
		object = *ptrToPtr;

		if (object->physics.movable) {
			//	object position [meters]
			//	= old position [meters] + (velocity [meters / timestep] * 1 timestep)
			object->physics.position += object->physics.velocity;

			//	Object velocity [meters / timestep]
			//	=	old velocity [meters / timestep]
			//		+ (acceleration [meters / timestep^2] * 1 timestep)
			object->physics.velocity += object->physics.acceleration;
		}
	}
}

/*	Object CRUD methods	*/

unsigned int ServerGameState::createObject(ObjectType type) {
	//	Create a new object with the given type in the relevant type-specific
	//	object SmartVector, add a reference to it in the global object
	//	SmartVector, and set the indices it receives in both as its type ID
	//	and global ID, respectively.

	unsigned int typeID, globalID;

	switch (type) {
		case ObjectType::Object:
			//	Create a new object of type Object in base_objects
			typeID = this->base_objects.push(Object());

			//	Add a reference to the new object in the global objects 
			//	SmartVector
			Object * newObject = this->base_objects.get(typeID);

			globalID = this->objects.push(newObject);

			//	Set object's type and global IDs
			newObject->globalID = globalID;
			newObject->typeID = typeID;
			break;
	}

	//	Return new object's type ID
	return typeID;
}

bool ServerGameState::removeObject(unsigned int global_id) {
	//	Check that the given object exists
	Object** ptrToPtr = this->objects.get(global_id);

	if (ptrToPtr == nullptr) {
		//	Object with the given index doesn't exist
		return false;
	}

	Object* object = *ptrToPtr;

	//	Remove object from the global objects SmartVector and from the
	//	type-specific Object vector it's in
	this->objects.remove(global_id);

	switch (object->type) {
		case ObjectType::Object:
			//	Remove this object from the base_objects SmartVector
			this->base_objects.remove(object->typeID);
			break;
	}

	return true;
}

Object* ServerGameState::getObject(unsigned int global_id) {
	Object** ptrToPtr = this->objects.get(global_id);

	if (ptrToPtr == nullptr) {
		return nullptr;
	}
	else {
		return *ptrToPtr;
	}
}

Object* ServerGameState::getBaseObject(unsigned int type_id) {
	return this->base_objects.get(type_id);
}

unsigned int ServerGameState::getTimestep() const {
	return this->timestep;
}

std::chrono::milliseconds ServerGameState::getTimestepLength() const {
	return this->timestep_length;
}

GamePhase ServerGameState::getPhase() const {
	return this->phase;
}

void ServerGameState::addPlayerToLobby(EntityID id, std::string name) {
	this->lobby.players[id] = name;
}

void ServerGameState::removePlayerFromLobby(EntityID id) {
	this->lobby.players.erase(id);
}

const std::unordered_map<EntityID, std::string>& ServerGameState::getLobbyPlayers() const {
	return this->lobby.players;
}

int ServerGameState::getLobbyMaxPlayers() const {
	return this->lobby.max_players;
}

std::string ServerGameState::to_string() {
	std::string representation = "{";
	representation += "\n\ttimestep:\t\t" + std::to_string(this->timestep);
	representation += "\n\ttimestep len:\t\t" + std::to_string(this->timestep_length.count());
	representation += "\n\tobjects: [\n";

	for (int i = 0; i < this->objects.size(); i++) {
		Object** ptrToPtr = this->objects.get(i);

		if (ptrToPtr == nullptr)
			continue;

		Object* object = *ptrToPtr;

		representation += object->to_string(2);

		if (i < this->objects.size() - 1) {
			representation += ",";
		}

		representation += "\n";
	}

	representation += "\t]\n}";

	return representation;
}