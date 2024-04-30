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

		case EventType::Jump: {
			auto jumpEvent = boost::get<JumpEvent>(event.data);
			obj = this->objects.getObject(jumpEvent.entity_to_move);
			if (obj->physics.velocity.y != 0) { break; }
			obj->physics.velocity += jumpEvent.movement * PLAYER_SPEED * 2.0f;
			break;
		}
		case EventType::HorizontalKeyDown: {	// if left/right key down, set the velocity to given 
			auto horizontalEvent = boost::get<HorizontalKeyDownEvent>(event.data);
			obj = this->objects.getObject(horizontalEvent.entity_to_move);
			obj->physics.velocity = obj->physics.velocity * glm::vec3(0.0f, 1.0f, 1.0f);
			obj->physics.velocity += horizontalEvent.movement * PLAYER_SPEED;
			break;
		}
		case EventType::VerticalKeyDown: {	// if up/down key down, set the velocity to given 
			auto verticalEvent = boost::get<VerticalKeyDownEvent>(event.data);
			obj = this->objects.getObject(verticalEvent.entity_to_move);
			obj->physics.velocity = obj->physics.velocity * glm::vec3(1.0f, 1.0f, 0.0f);
			obj->physics.velocity += verticalEvent.movement * PLAYER_SPEED;
			break;
		}
		case EventType::HorizontalKeyUp: { // if key is off, stop moving horizontally
			auto stopHorizontalEvent = boost::get<HorizontalKeyUpEvent>(event.data);
			obj = this->objects.getObject(stopHorizontalEvent.entity_to_move);
			obj->physics.velocity = obj->physics.velocity * glm::vec3(0.0f, 1.0f, 1.0f);
			break;
		}
		case EventType::VerticalKeyUp: { // if key is off, stop moving vertically
			auto stopVerticalEvent = boost::get<VerticalKeyUpEvent>(event.data);
			obj = this->objects.getObject(stopVerticalEvent.entity_to_move);
			obj->physics.velocity = obj->physics.velocity * glm::vec3(1.0f, 1.0f, 0.0f);
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
			//TODO : check for collision at position to move, if so, dont change position

			object->physics.shared.position += object->physics.velocity;

			// update gravity factor
			if ((object->physics.shared.position).y >= 0) {
				object->physics.velocity.y -= GRAVITY;
			} else {
				object->physics.velocity.y = 0.0f;
			}
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