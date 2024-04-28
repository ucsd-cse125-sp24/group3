#include "server/game/servergamestate.hpp"
#include "shared/game/sharedgamestate.hpp"

/*	Constructors and Destructors	*/

ServerGameState::ServerGameState(GamePhase start_phase, GameConfig config) {
	this->phase = start_phase;
	this->timestep = FIRST_TIMESTEP;
	this->timestep_length = config.game.timestep_length_ms;
	this->lobby.max_players = config.server.max_players;
}

ServerGameState::ServerGameState(GamePhase start_phase) {
	this->phase = start_phase;
	this->timestep = FIRST_TIMESTEP;
	this->timestep_length = TIMESTEP_LEN;
	this->lobby.max_players = MAX_PLAYERS;
}

ServerGameState::ServerGameState() : ServerGameState(GamePhase::LOBBY) {}

ServerGameState::~ServerGameState() {}

/*	SharedGameState generation	*/
SharedGameState ServerGameState::generateSharedGameState() {
	//	Create a new SharedGameState instance and populate it with this
	//	ServerGameState's data
	SharedGameState shared;

	//	Initialize object vector
	shared.objects = this->objects.toShared();

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

void ServerGameState::update(const EventList& events) {

	for (const auto& [src_eid, event] : events) { // cppcheck-suppress unusedVariable
		std::cout << event << std::endl;
		Object* obj;
        switch (event.type) {

		case EventType::MoveRelative: {	// if key down, set the velocity to given 
			auto moveRelativeEvent = boost::get<MoveRelativeEvent>(event.data);
			obj = this->objects.getObject(moveRelativeEvent.entity_to_move);
			obj->physics.velocity = moveRelativeEvent.movement;
			break;
		}
		case EventType::MoveKeyUp: { // if key is off, stop moving	
			auto stopMove = boost::get<MoveKeyUpEvent>(event.data);
			obj = this->objects.getObject(stopMove.entity_to_move);
			obj->physics.velocity = glm::vec3(0.0f, 0.0f, 0.0f);
			break;
		}

		default: {}
		//     std::cerr << "Unimplemented EventType (" << event.type << ") received" << std::endl;
        }
    }

	//	TODO: fill update() method with updating object movement
	useItem();
	updateMovement();
	
	//	Increment timestep
	this->timestep++;
}

void ServerGameState::updateMovement() {
	//	Update all movable objects' movement

	//	Iterate through all objects in the ServerGameState and update their
	//	positions and velocities if they are movable.

	SmartVector<Object*> gameObjects = this->objects.getObjects();
	for (int i = 0; i < gameObjects.size(); i++) {
		Object* object = gameObjects.get(i);

		if (object == nullptr)
			continue;
		
		if (object->physics.movable) {
			//	object position [meters]
			//	= old position [meters] + (velocity [meters / timestep] * 1 timestep)
			object->physics.shared.position += object->physics.velocity;


			//	Object velocity [meters / timestep]
			//	=	old velocity [meters / timestep]
			//		+ (acceleration [meters / timestep^2] * 1 timestep)
			//object->physics.velocity += object->physics.acceleration;
		}
	}
}

void ServerGameState::useItem() {
	// Update whatever is necesssary for item
	// This method may need to be broken down for different types
	// of item types

	SmartVector<Item*> items = this->objects.getItems();
	for (int i = 0; i < items.size(); i++) {
		Item* item = items.get(i);

		if (item == nullptr)
			continue;
	}
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

void ServerGameState::setPhase(GamePhase phase) {
	this->phase = phase;
}

void ServerGameState::addPlayerToLobby(EntityID id, const std::string& name) {
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

	SmartVector<Object*> gameObjects = this->objects.getObjects();

	for (int i = 0; i < gameObjects.size(); i++) {
		Object* object = gameObjects.get(i);

		if (object == nullptr)
			continue;

		representation += object->to_string(2);

		if (i < gameObjects.size() - 1) {
			representation += ",";
		}

		representation += "\n";
	}

	representation += "\t]\n}";

	return representation;
}