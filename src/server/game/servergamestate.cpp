#include "server/game/servergamestate.hpp"
#include "shared/game/sharedgamestate.hpp"
#include "server/game/spiketrap.hpp"
#include "server/game/arrowtrap.hpp"
#include "server/game/floorspike.hpp"
#include "server/game/fakewall.hpp"
#include "shared/utilities/root_path.hpp"
#include "shared/utilities/time.hpp"

#include <fstream>

/*	Constructors and Destructors	*/

ServerGameState::ServerGameState() : ServerGameState(getDefaultConfig()) {}

ServerGameState::ServerGameState(GameConfig config) {
	this->phase = GamePhase::LOBBY;
	this->timestep = FIRST_TIMESTEP;
	this->timestep_length = config.game.timestep_length_ms;
	this->lobby.max_players = config.server.max_players;
	this->lobby.name = config.server.lobby_name;

	this->maps_directory = config.game.maze.directory;
	this->maze_file = config.game.maze.maze_file;

	//	Load maze (Note: This only happens in THIS constructor! All other
	//	ServerGameState constructors MUST call this constructor to load the
	//	maze environment from a file)
	this->loadMaze();
}

ServerGameState::ServerGameState(GamePhase start_phase) 
	: ServerGameState(getDefaultConfig()) {
	this->phase = start_phase;
}

ServerGameState::ServerGameState(GamePhase start_phase, const GameConfig& config)
	: ServerGameState(config) {
	this->phase = start_phase;
}

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
		// skip any events from dead players
		auto player = dynamic_cast<Player*>(this->objects.getObject(src_eid));
		if (player != nullptr && !player->info.is_alive) {
			continue;
		}

		Object* obj;
	
        switch (event.type) {
		case EventType::ChangeFacing: {
            auto changeFacingEvent = boost::get<ChangeFacingEvent>(event.data);
            Object* objChangeFace = this->objects.getObject(changeFacingEvent.entity_to_change_face);
            objChangeFace->physics.shared.facing = changeFacingEvent.facing;
            break;
		}
	
		case EventType::StartAction: {
			auto startAction = boost::get<StartActionEvent>(event.data);
			obj = this->objects.getObject(startAction.entity_to_act);
			//switch case for action (currently using keys)
			switch (startAction.action) {
			case ActionType::MoveCam: {
				obj->physics.velocity.x = (startAction.movement * PLAYER_SPEED).x;
				obj->physics.velocity.z = (startAction.movement * PLAYER_SPEED).z;
				break;
			}
			case ActionType::Jump: {
				if (obj->physics.velocity.y != 0) { break; }
				obj->physics.velocity.y += (startAction.movement * JUMP_SPEED / 2.0f).y;
				break;
			}
			case ActionType::Sprint: {
				obj->physics.velocityMultiplier = glm::vec3(1.5f, 1.1f, 1.5f);
				break;
			}
			default: {}
			}
			break;
		}

		case EventType::StopAction: {
			auto stopAction = boost::get<StopActionEvent>(event.data);
			obj = this->objects.getObject(stopAction.entity_to_act);
			//switch case for action (currently using keys)
			switch (stopAction.action) {
			case ActionType::MoveCam: {
				obj->physics.velocity.x = 0.0f;
				obj->physics.velocity.z = 0.0f;
				break;
			}
			case ActionType::Sprint: {
				obj->physics.velocityMultiplier = glm::vec3(1.0f, 1.0f, 1.0f);
				break;
			}
			default: { break; }
			}
			break;
		}
	
        case EventType::MoveRelative:
		{
			//currently just sets the velocity to given 
            auto moveRelativeEvent = boost::get<MoveRelativeEvent>(event.data);
            Object* objMoveRel = this->objects.getObject(moveRelativeEvent.entity_to_move);
            objMoveRel->physics.velocity += moveRelativeEvent.movement;
            break;
		}

		// default:
		//     std::cerr << "Unimplemented EventType (" << event.type << ") received" << std::endl;
        }
    }

	//	TODO: fill update() method with updating object movement
	useItem();
	updateMovement();
	updateTraps();
	handleDeaths();
	handleRespawns();
	
	//	Increment timestep
	this->timestep++;
}

void ServerGameState::updateMovement() {
	//	Update all movable objects' movement

	//	Iterate through all objects in the ServerGameState and update their
	//	positions and velocities if they are movable.

	// If objects are moving too fast, we split their movement into NUM_INCREMENTAL_STEPS smaller steps
	const int NUM_INCREMENTAL_STEPS = 5;
	// This is the threshold that determines if we need to do incremental steps for the movement
	// if the magnitude of movementStep is greater than this value, then we do incremental steps for the movement
	const float SINGLE_MOVE_THRESHOLD = 0.20f;

	// Don't set this directly, it is determined by NUM_INCREMENTAL_STEPS, and is just the reciprical
	const float INCREMENTAL_MOVE_RATIO = 1.0f / NUM_INCREMENTAL_STEPS;

	SmartVector<Object*> gameObjects = this->objects.getObjects();
	for (int i = 0; i < gameObjects.size(); i++) {
		Object* object = gameObjects.get(i);

		if (object == nullptr)
			continue;

		bool collided = false; // cppcheck-suppress variableScope
		bool collidedX = false; // cppcheck-suppress variableScope
		bool collidedZ = false; // cppcheck-suppress variableScope

		if (object->physics.movable) {
			// Check for collision at position to move, if so, dont change position
			// O(n^2) naive implementation of collision detection
			glm::vec3 totalMovementStep = object->physics.velocity * object->physics.velocityMultiplier;
			glm::vec3 movementStep;
			int numSteps = 0;
			if (glm::length(totalMovementStep) > SINGLE_MOVE_THRESHOLD) {
				movementStep = INCREMENTAL_MOVE_RATIO * totalMovementStep;
			}
			else {
				movementStep = totalMovementStep;
				numSteps = NUM_INCREMENTAL_STEPS - 1;
			}

			while (numSteps < NUM_INCREMENTAL_STEPS) {
				numSteps++;
				// Run collision detection movement if it has a collider
				if (object->physics.collider != Collider::None) {
					//object->physics.shared.corner += movementStep;
					this->objects.moveObject(object, object->physics.shared.corner
						+ movementStep);

					// TODO : for possible addition for smooth collision detection, but higher computation
					// 1) when moving collider, seperate the movement into 4 steps ex:(object->physics.velocity * object->physics.acceleration) / 4
					//    Then, take the most steps possible (mario 64 handles it like this)
					// 2) Using raycasting

					/*for (int j = 0; j < gameObjects.size(); j++) {
						if (i == j) { continue; }
						Object* otherObj = gameObjects.get(j);*/

					/*std::cout << "Cells occupied by object: ";

					for (glm::vec2 cellPos : object->gridCellPositions) {
						std::cout << "( " << cellPos.x << ", " << cellPos.y << ") ";
					}
					std::cout << std::endl;

					std::cout << "List of grid cell position keys: " << std::endl;
					for (auto it : this->objects.cellToObjects) {
						std::cout << glm::to_string(it.first) << " ";
					}*/

					std::cout << std::endl;

					//	Iterate through the object's occupied grid cells
					for (glm::ivec2 cellPos : object->gridCellPositions) {
						//	Get vector of Object pointers that occupy this cell
						std::vector<Object*>& objectsInCell =
							this->objects.cellToObjects.at(cellPos);

						//	Iterate through all objects in this grid cell
						for (Object* otherObj : objectsInCell) {
							if (otherObj->globalID == object->globalID) { continue; }
							if (otherObj->physics.collider == Collider::None) { continue; }

							if (detectCollision(object->physics, otherObj->physics)) {
								

								if (otherObj->type == ObjectType::FloorSpike) {
									object->doCollision(otherObj, this);
									otherObj->doCollision(object, this);
									continue;
								}


								collided = true;

								// Check x-axis collision
								//object->physics.shared.corner.z -= movementStep.z;
								this->objects.moveObject(object, glm::vec3(
									object->physics.shared.corner.x,
									object->physics.shared.corner.y,
									object->physics.shared.corner.z - movementStep.z
								));
								if (detectCollision(object->physics, otherObj->physics)) {
									collidedX = true;
								}

								// Check z-axis collision
								/*object->physics.shared.corner.z += movementStep.z;
								object->physics.shared.corner.x -= movementStep.x;*/
								this->objects.moveObject(object, glm::vec3(
									object->physics.shared.corner.x - movementStep.x,
									object->physics.shared.corner.y,
									object->physics.shared.corner.z + movementStep.z
								));
								if (detectCollision(object->physics, otherObj->physics)) {
									collidedZ = true;
								}
								//object->physics.shared.corner.x += movementStep.x;
								this->objects.moveObject(object, glm::vec3(
									object->physics.shared.corner.x + movementStep.x,
									object->physics.shared.corner.y,
									object->physics.shared.corner.z
								));

								object->doCollision(otherObj, this);
								otherObj->doCollision(object, this);
							}
						}
					}

					if (collidedX) {
						//object->physics.shared.corner.x -= movementStep.x;
						this->objects.moveObject(object, glm::vec3(
							object->physics.shared.corner.x - movementStep.x,
							object->physics.shared.corner.y,
							object->physics.shared.corner.z
						));
					}

					if (collidedZ) {
						//object->physics.shared.corner.z -= movementStep.z;
						this->objects.moveObject(object, glm::vec3(
							object->physics.shared.corner.x,
							object->physics.shared.corner.y,
							object->physics.shared.corner.z - movementStep.z
						));
					}
				}
				else {
					//object->physics.shared.corner += movementStep;
					this->objects.moveObject(object, 
						object->physics.shared.corner + movementStep);
				}

				if (object->physics.shared.corner.y <= 0) {
					object->physics.shared.corner.y = 0;
				}

				if (collidedX && collidedZ) {
					break; // don't need to do the further movement steps until we reach totalmovement step
				}
			}

			// update gravity factor
			if ((object->physics.shared.corner).y > 0) {
				object->physics.velocity.y -= GRAVITY;
			}
			else {
				object->physics.velocity.y = 0.0f;
			}
		}
	}
}

//void ServerGameState::updateMovement() {
//	//	Update all movable objects' movement
//
//	//	Iterate through all objects in the ServerGameState and update their
//	//	positions and velocities if they are movable.
//
//	//	If objects are moving too fast, we split their movmeent into
//	//	NUM_INCREMENTAL_STEPS
//	const int NUM_INCREMENTAL_STEPS = 5;
//	//	This is the threshold that determines if we need to use incremental
//	//	steps for the movement.
//	//	If the magnitude of movementStep is greater than this value, then we use
//	//	incremental steps for the movement.
//	const float SINGLE_MOVE_THRESHOLD = 0.20f;
//
//	//	Don't set this directly, it is determined by NUM_INCREMENTAL_STEPS, and
//	//	is just the reciprocal
//	const float INCREMENTAL_MOVE_RATIO = 1.0f / NUM_INCREMENTAL_STEPS;
//
//	SmartVector<Object*> gameObjects = this->objects.getObjects();
//	for (int i = 0; i < gameObjects.size(); i++) {
//		Object* object = gameObjects.get(i);
//
//		if (object == nullptr || !(object->physics.movable))
//			continue;
//
//		bool collidedX, collidedZ;
//		collidedX = collidedZ = false;
//
//		//	Compute object's movement step
//		glm::vec3 totalMovementStep = 
//			object->physics.velocity * object->physics.velocityMultiplier;
//		glm::vec3 movementStep;
//		glm::vec3 newPosition;
//		int numSteps = 0;
//		if (object->physics.collider == Collider::None
//			|| glm::length(totalMovementStep) <= SINGLE_MOVE_THRESHOLD) {
//			movementStep = totalMovementStep;
//			numSteps = NUM_INCREMENTAL_STEPS - 1;
//		}
//		else {
//			movementStep = INCREMENTAL_MOVE_RATIO * totalMovementStep;
//		}
//
//		while (numSteps < NUM_INCREMENTAL_STEPS) {
//			numSteps++;
//
//			//	Compute new position
//			newPosition = object->physics.shared.corner + movementStep;
//
//			//	Move object to new position
//			this->objects.moveObject(object, newPosition);
//
//			//	Perform collision detection if it the object has a collider
//			if (object->physics.collider != Collider::None) {
//				//	Iterate through object's occupied grid cells
//				for (glm::vec2 cellPos : object->gridCellPositions) {
//					//	Get vector of Object pointers that occupy this cell
//					std::vector<Object*>& objectsInCell =
//						this->objects.cellToObjects.at(cellPos);
//
//					//	Iterate through all objects in this grid cell
//					for (Object* otherObj : objectsInCell) {
//						//	Skip if this object is the current object or if this
//						//	object has no collider
//						if (object->globalID == otherObj->globalID
//							|| otherObj->physics.collider == Collider::None)
//							continue;
//
//						//	Check for collision
//						if (detectCollision(object->physics, otherObj->physics)) {
//							std::cout << "Detected collision! EID: "
//								<< object->globalID << " with EID " << otherObj->globalID << std::endl;
//							//	If other object is a floor spike trap, handle
//							//	collision differently
//							if (otherObj->type == ObjectType::FloorSpike) {
//								object->doCollision(otherObj, this);
//								otherObj->doCollision(object, this);
//								continue;
//							}
//
//							//	Normal collision resolution follows
//
//							//	Check for x-axis collision
//							this->objects.moveObject(object, glm::vec3(
//								newPosition.x,
//								newPosition.y,
//								newPosition.z - movementStep.z
//							));
//
//							if (detectCollision(object->physics, otherObj->physics)) {
//								collidedX = true;
//							}
//
//							//	Check for z-axis collision
//							this->objects.moveObject(object, glm::vec3(
//								newPosition.x - movementStep.x,
//								newPosition.y,
//								newPosition.z
//							));
//
//							if (detectCollision(object->physics, otherObj->physics)) {
//								collidedZ = true;
//							}
//
//							//	Update object's position
//							this->objects.moveObject(object, newPosition);
//
//							//	Collision response
//							object->doCollision(otherObj, this);
//							otherObj->doCollision(object, this);
//						}
//					}
//				}
//
//				//	Update new position if collided
//				if (collidedX || collidedZ) {
//					if (collidedX) {
//						newPosition.x -= movementStep.x;
//					}
//					if (collidedZ) {
//						newPosition.z -= movementStep.z;
//					}
//
//					//	Update object's position to updated new position
//					this->objects.moveObject(object, newPosition);
//				}
//
//				if (collidedX && collidedZ) {
//					//	This means the object is stopped;
//					//	skip any additional incremental steps
//					break;
//				}
//			}
//		}
//
//		if (object->physics.shared.corner.y > 0) {
//			//	Object is in the air - add gravity to velocity
//			object->physics.velocity.y -= GRAVITY;
//		}
//		else {
//			object->physics.velocity.y = 0.0f;
//		}
//
//		//	Clamp object position to floor if y position is less 0
//		if (object->physics.shared.corner.y < 0) {
//			//	Technically, we don't need to call moveObject() here since we're
//			//	only updating the object's y position (doesn't affect occupied
//			//	grid cells)
//			this->objects.moveObject(object, glm::vec3(
//				object->physics.shared.corner.x,
//				0,
//				object->physics.shared.corner.z
//			));
//		}
//	}
//}

//void ServerGameState::updateMovement() {
//	//	Update all movable objects' movement
//
//	//	Iterate through all objects in the ServerGameState and update their
//	//	positions and velocities if they are movable.
//
//	// If objects are moving too fast, we split their movement into NUM_INCREMENTAL_STEPS smaller steps
//	const int NUM_INCREMENTAL_STEPS = 5;
//	// This is the threshold that determines if we need to do incremental steps for the movement
//	// if the magnitude of movementStep is greater than this value, then we do incremental steps for the movement
//	const float SINGLE_MOVE_THRESHOLD = 0.20f;
//
//	// Don't set this directly, it is determined by NUM_INCREMENTAL_STEPS, and is just the reciprical
//	const float INCREMENTAL_MOVE_RATIO = 1.0f / NUM_INCREMENTAL_STEPS;
//
//	SmartVector<Object*> gameObjects = this->objects.getObjects();
//	for (int i = 0; i < gameObjects.size(); i++) {
//		Object* object = gameObjects.get(i);
//
//		if (object == nullptr)
//			continue;
//		
//		bool collided = false; // cppcheck-suppress variableScope
//		bool collidedX = false; // cppcheck-suppress variableScope
//		bool collidedZ = false; // cppcheck-suppress variableScope
//
//		if (object->physics.movable) {
//			// Check for collision at position to move, if so, dont change position
//			// O(n^2) naive implementation of collision detection
//			glm::vec3 totalMovementStep = object->physics.velocity * object->physics.velocityMultiplier;
//			glm::vec3 movementStep;
//			int numSteps = 0;
//			if (glm::length(totalMovementStep) > SINGLE_MOVE_THRESHOLD) {
//				movementStep = INCREMENTAL_MOVE_RATIO * totalMovementStep;
//			} else {
//				movementStep = totalMovementStep;
//				numSteps = NUM_INCREMENTAL_STEPS - 1;
//			}
//
//			while (numSteps < NUM_INCREMENTAL_STEPS) {
//				numSteps++;
//				// Run collision detection movement if it has a collider
//				if (object->physics.collider != Collider::None) {
//					object->physics.shared.corner += movementStep;
//
//					// TODO : for possible addition for smooth collision detection, but higher computation
//					// 1) when moving collider, seperate the movement into 4 steps ex:(object->physics.velocity * object->physics.acceleration) / 4
//					//    Then, take the most steps possible (mario 64 handles it like this)
//					// 2) Using raycasting
//
//					for (int j = 0; j < gameObjects.size(); j++) {
//						if (i == j) { continue; }
//						Object* otherObj = gameObjects.get(j);
//
//						if (otherObj->physics.collider == Collider::None) { continue; }
//
//						if (detectCollision(object->physics, otherObj->physics)) {
//							
//							if (otherObj->type == ObjectType::FloorSpike) {
//								object->doCollision(otherObj, this);
//								otherObj->doCollision(object, this);
//								continue;
//							}
//							
//
//							collided = true;
//
//							// Check x-axis collision
//							object->physics.shared.corner.z -= movementStep.z;
//							if (detectCollision(object->physics, otherObj->physics)) {
//								collidedX = true;
//							}
//
//							// Check z-axis collision
//							object->physics.shared.corner.z += movementStep.z;
//							object->physics.shared.corner.x -= movementStep.x;
//							if (detectCollision(object->physics, otherObj->physics)) {
//								collidedZ = true;
//							}
//							object->physics.shared.corner.x += movementStep.x;
//
//							object->doCollision(otherObj, this);
//							otherObj->doCollision(object, this);
//						}
//					}
//
//					if (collidedX) {
//						object->physics.shared.corner.x -= movementStep.x;
//					}
//
//					if (collidedZ) {
//						object->physics.shared.corner.z -= movementStep.z;
//					}
//				}
//				else {
//					object->physics.shared.corner += movementStep;
//				}
//
//				if (object->physics.shared.corner.y <= 0) {
//					object->physics.shared.corner.y = 0;
//				}
//
//				if (collidedX && collidedZ) {
//					break; // don't need to do the further movement steps until we reach totalmovement step
//				}
//			}
//
//			// update gravity factor
//			if ((object->physics.shared.corner).y > 0) {
//				object->physics.velocity.y -= GRAVITY;
//			}
//			else {
//				object->physics.velocity.y = 0.0f;
//			}
//		}
//	}
//}

void ServerGameState::useItem() {
	// Update whatever is necesssary for item
	// This method may need to be broken down for different types
	// of item types

	SmartVector<Item*> items = this->objects.getItems();
	for (int i = 0; i < items.size(); i++) {
		const Item* item = items.get(i);

		if (item == nullptr)
			continue;
	}
}

void ServerGameState::updateTraps() {
	// check for activations

	// This object moved, so we should check to see if a trap should trigger because of it
	auto traps = this->objects.getTraps();
	for (int i = 0; i < traps.size(); i++) {
		auto trap = traps.get(i);
		if (trap == nullptr) { continue; } // unsure if i need this?
		if (trap->shouldTrigger(*this)) {
			trap->trigger(*this);
		}
        if (trap->shouldReset(*this)) {
            trap->reset(*this);
        }
	}
}

void ServerGameState::handleDeaths() {
	// TODO: also handle enemy deaths
	// unsure of the best way to do this right now
	// ideally we would be able to get an array of all of the creatures
	// but the current interface of the object manager doesn't really let you do that
	// easily

	// thinking that you might have to handle enemies differently either way because
	// they wont have a SharedPlayerInfo and respawn time stuff they need to
	auto players = this->objects.getPlayers();
	for (int p = 0; p < players.size(); p++) {
		auto player = players.get(p);
		if (player == nullptr) continue;

		if (player->stats.health.current() <= 0 && player->info.is_alive) {
			player->info.is_alive = false;
			player->info.respawn_time = getMsSinceEpoch() + 5000; // currently hardcode to wait 5s
		}
	}
}

void ServerGameState::handleRespawns() {
	auto players = this->objects.getPlayers();
	for (int p = 0; p < players.size(); p++) {
		auto player = players.get(p);
		if (player == nullptr) continue;

		if (!player->info.is_alive) {
			if (getMsSinceEpoch() >= player->info.respawn_time) {
				player->physics.shared.corner = this->getGrid().getRandomSpawnPoint();
				player->info.is_alive = true;
				player->stats.health.adjustBase(player->stats.health.max());
			}
		}
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

const Lobby& ServerGameState::getLobby() const {
	return this->lobby;
}

/*	Maze initialization	*/

void ServerGameState::loadMaze() {
	//	Step 1:	Attempt to open maze file for reading.

	//	Generate maze file path
	auto maze_file_path = 
		getRepoRoot() / this->maps_directory / this->maze_file;

	std::ifstream file;

	file.open(maze_file_path.string(), std::ifstream::in);

	//	Check that maze file was successfully opened
	assert(file.is_open());

	//	Step 2:	Determine number of rows and columns in the maze.

	int rows, columns;

	//	Character buffer that stores a single line of the input maze file
	//	(extra character for null terminator)
	char buffer[MAX_MAZE_COLUMNS + 1];

	//	Get number of columns

	//	Read the first line and use its length to get the number of columns
	file.getline(buffer, MAX_MAZE_COLUMNS + 1);

	columns = file.gcount();

	//	If end of file isn't reached, file.gcount() includes the newline
	//	character at the end of the first line; remove it from the column
	//	count.
	if (!file.eof()) {
		columns -= 1;
	}

	std::cout << "Number of columns: " << columns << std::endl;

	//	Get number of rows

	//	Rows is at least one due to the above getline() call
	rows = 1;
	while (!file.eof()) {
		file.getline(buffer, MAX_MAZE_COLUMNS + 1);

		//	Assert if number of columns read doesn't match the number of columns
		//	in the first row
		int numColumns = file.gcount();

		//	If end-of-file not reached, the gcount() contains the newline
		//	character at the end of the current row; remove it from the current
		//	column count.
		if (!file.eof())
			numColumns -= 1;

		std::cout << "row " << rows << ": Num columns: " << numColumns << std::endl;

		assert(numColumns == columns);

		rows++;
	}


	std::cout << "Number of rows: " << rows << std::endl;

	//	Initialize Grid with the specified rows and columns
	this->grid = Grid(rows, columns);

	//	Step 3:	Fill Grid with GridCells corresponding to characters in the maze
	//	file.

	//	Reset file position
	file.seekg(file.beg);

	//	Populate Grid
	for (int row = 0; row < this->grid.getRows(); row++) {
		//	Read row from file
		file.getline(buffer, this->grid.getColumns() + 1);

		for (int col = 0; col < this->grid.getColumns(); col++) {
			char c = buffer[col];

			//	Identify CellType from character
			CellType type = charToCellType(c);

			//	Crash if CellType is unknown
			assert(type != CellType::Unknown);

			//	Create new GridCell
			this->grid.addCell(col, row, type);
		}
	}

	//	Step 4:	Close the maze file.

	file.close();

	//	Verify that there's at least one spawn point
	size_t num_spawn_points = this->grid.getSpawnPoints().size();
	assert(num_spawn_points > 0);

	//	Step 5:	Add floor and ceiling SolidSurfaces.

	// Create Floor
	this->objects.createObject(new SolidSurface(false, Collider::None, SurfaceType::Floor, 
		glm::vec3(0.0f, -0.1f, 0.0f),
		glm::vec3(this->grid.getColumns() * Grid::grid_cell_width, 0.1,
			this->grid.getRows() * Grid::grid_cell_width)
	));
	// Create Ceiling
	this->objects.createObject(new SolidSurface(false, Collider::None, SurfaceType::Ceiling, 
		glm::vec3(0.0f, MAZE_CEILING_HEIGHT, 0.0f),
		glm::vec3(this->grid.getColumns() * Grid::grid_cell_width, 0.1,
			this->grid.getRows() * Grid::grid_cell_width)
	));

	//	Step 6:	For each GridCell, add an object (if not empty) at the 
	//	GridCell's position.
	for (int row = 0; row < this->grid.getRows(); row++) {
		for (int col = 0; col < this->grid.getColumns(); col++) {
			GridCell* cell = this->grid.getCell(col, row);

			switch (cell->type) {
				case CellType::ArrowTrap: {
					glm::vec3 dimensions(
						Grid::grid_cell_width / 2,
						0.5f,
						Grid::grid_cell_width / 2
					);
					glm::vec3 corner(
						cell->x * Grid::grid_cell_width,
						1.0f,
						cell->y * Grid::grid_cell_width
					);
					this->objects.createObject(new ArrowTrap(corner, dimensions));
					break;
				}
				case CellType::SpikeTrap: {
                    const float HEIGHT_SHOWING = 0.5;
					glm::vec3 dimensions(
						Grid::grid_cell_width,
						MAZE_CEILING_HEIGHT,
						Grid::grid_cell_width
					);
					glm::vec3 corner(
						cell->x * Grid::grid_cell_width,
						MAZE_CEILING_HEIGHT - HEIGHT_SHOWING, 
						cell->y * Grid::grid_cell_width
					);

					this->objects.createObject(new SpikeTrap(corner, dimensions));
					break;
				}
				case CellType::Enemy: {
					this->objects.createObject(new Enemy(
						this->grid.gridCellCenterPosition(cell), glm::vec3(0.0f)));
					break;
				}
				case CellType::Wall:
				case CellType::FakeWall: {
					glm::vec3 dimensions(
						Grid::grid_cell_width,
						MAZE_CEILING_HEIGHT,
						Grid::grid_cell_width
					);
					glm::vec3 corner(
						cell->x * Grid::grid_cell_width,
						0.0f, 
						cell->y * Grid::grid_cell_width
					);

					if (cell->type == CellType::FakeWall) {
						this->objects.createObject(new FakeWall(corner, dimensions));
					} else if (cell->type == CellType::Wall) {
						this->objects.createObject(new SolidSurface(false, Collider::Box, SurfaceType::Wall, corner, dimensions));
					}
					break;
				}
				case CellType::FloorSpikeFull:
				case CellType::FloorSpikeHorizontal:
				case CellType::FloorSpikeVertical: {
					glm::vec3 corner(
						cell->x * Grid::grid_cell_width,
						0.0f, 
						cell->y * Grid::grid_cell_width
					);

					FloorSpike::Orientation orientation;
					if (cell->type == CellType::FloorSpikeFull) {
						orientation = FloorSpike::Orientation::Full;
					} else if (cell->type == CellType::FloorSpikeHorizontal) {
						orientation = FloorSpike::Orientation::Horizontal;
						corner.z += Grid::grid_cell_width * 0.25f;
					} else {
						orientation = FloorSpike::Orientation::Vertical;
						corner.x += Grid::grid_cell_width * 0.25f;
					}

					this->objects.createObject(new FloorSpike(corner, orientation, Grid::grid_cell_width));
					break;
				}
			}
		}
	}
}

Grid& ServerGameState::getGrid() {
	return this->grid;
}

/*	Debugger Methods	*/

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