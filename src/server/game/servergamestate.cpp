#include "server/game/servergamestate.hpp"
#include "shared/game/sharedgamestate.hpp"
#include "server/game/boxcollider.hpp"
#include "server/game/spiketrap.hpp"
#include "server/game/potion.hpp"
#include "shared/utilities/root_path.hpp"
#include "server/game/constants.hpp"

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

//ServerGameState::ServerGameState(GamePhase start_phase, GameConfig config) 
//	: ServerGameState(DEFAULT_MAZE_FILE) {
//	this->phase = start_phase;
//	this->timestep_length = config.game.timestep_length_ms;
//	this->lobby.max_players = config.server.max_players;
//}
//
//ServerGameState::ServerGameState(GamePhase start_phase) 
//	: ServerGameState(DEFAULT_MAZE_FILE) {
//	this->phase = start_phase;
//}
//
//ServerGameState::ServerGameState() : ServerGameState(DEFAULT_MAZE_FILE) {}
//
//ServerGameState::ServerGameState(std::string maze_file) {
//	this->phase = GamePhase::LOBBY;
//	this->timestep = FIRST_TIMESTEP;
//	this->timestep_length = TIMESTEP_LEN;
//	this->lobby.max_players = MAX_PLAYERS;
//	this->maze_file = maze_file;
//
//	//	Load maze (Note: This only happens in THIS constructor! All other
//	//	ServerGameState constructors MUST call this constructor to load the
//	//	maze environment from a file)
//	this->loadMaze();
//}
//
//ServerGameState::ServerGameState(GamePhase start_phase, GameConfig config, 
//	std::string maze_file) : ServerGameState(maze_file) {
//	this->phase = start_phase;
//	this->timestep_length = config.game.timestep_length_ms;
//	this->lobby.max_players = config.server.max_players;
//}

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
		//std::cout << event << std::endl;
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
		case EventType::UseItem:
		{
			auto useItemEvent = boost::get<UseItemEvent>(event.data);
			Object* obj = this->objects.getObject(useItemEvent.playerEID);
			Player* player = this->objects.getPlayer(obj->typeID);
			if (player->inventory.at(useItemEvent.itemNum) != nullptr) {
				player->useItem(useItemEvent.itemNum);
			}
			break;
		}

		// default:
		//     std::cerr << "Unimplemented EventType (" << event.type << ") received" << std::endl;
        }
    }

	//	TODO: fill update() method with updating object movement
	updateItem();
	updateMovement();
	updateTraps();
	
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
		
		bool collided = false; // cppcheck-suppress variableScope
		bool collidedX = false; // cppcheck-suppress variableScope
		bool collidedZ = false; // cppcheck-suppress variableScope

		if (object->physics.movable) {
			// Check for collision at position to move, if so, dont change position
			// O(n^2) naive implementation of collision detection
			Collider* currentCollider = object->physics.boundary;
			glm::vec3 movementStep = object->physics.velocity * object->physics.velocityMultiplier;

			// Run collision detection movement if it has a collider
			if (currentCollider != NULL) {
				currentCollider->corner += movementStep; // only move collider to check

				// TODO : for possible addition for smooth collision detection, but higher computation
				// 1) when moving collider, seperate the movement into 4 steps ex:(object->physics.velocity * object->physics.acceleration) / 4
				//    Then, take the most steps possible (mario 64 handles it like this)
				// 2) Using raycasting

				for (int j = 0; j < gameObjects.size(); j++) {
					if (i == j) { continue; }
					Object* otherObj = gameObjects.get(j);
					Collider* otherCollider = otherObj->physics.boundary;

					if (otherCollider == NULL) { continue; }

					if (currentCollider->detectCollision(otherCollider)) {
						collided = true;

						// If player colliding with items, add it to inventory
						if (object->type == ObjectType::Player && otherObj->type == ObjectType::Potion) {
							Player* player = this->objects.getPlayer(object->typeID);
							Item* item = this->objects.getItem(otherObj->typeID);

							if (player->inventory.size() < MAX_ITEMS) {
								for (int x : {1,2,3,4,5}) {
									if (player->inventory.contains(i)) {
										player->inventory[i] = item;
										break;
									}
								}

								item->iteminfo.held = true;
								item->physics.boundary = NULL;
								continue;
							}
						}

						// Check x-axis collision
						currentCollider->corner.z -= movementStep.z;
						if (currentCollider->detectCollision(otherCollider)) {
							collidedX = true;
						}

						// Check z-axis collision
						currentCollider->corner.z += movementStep.z;
						currentCollider->corner.x -= movementStep.x;
						if (currentCollider->detectCollision(otherCollider)) {
							collidedZ = true;
						}
						currentCollider->corner.x += movementStep.x;
					}
				}

				// Move object if no collision detected
				if (!collided) {
					object->physics.shared.position += movementStep;
					object->physics.shared.corner += movementStep;
				}
				// Revert collider if collided
				// Seperated for x/z axis collisions
				else {
					if (!collidedX) {
						object->physics.shared.position.x += movementStep.x;
						object->physics.shared.corner.x += movementStep.x;
					}
					else {
						currentCollider->corner.x -= movementStep.x;
					}

					if (!collidedZ) {
						object->physics.shared.position.z += movementStep.z;
						object->physics.shared.corner.z += movementStep.z;
					}
					else {
						currentCollider->corner.z -= movementStep.z;
					}
					object->physics.shared.position.y += movementStep.y;
					object->physics.shared.corner.y += movementStep.y;

				}

				// update gravity factor
				if ((object->physics.shared.corner).y >= 0) {
					object->physics.velocity.y -= GRAVITY;
				}
				else {
					object->physics.velocity.y = 0.0f;
				}
			}

			// if current object do not have a collider / this shouldn't happen though
			else {
				object->physics.shared.position += movementStep;
				object->physics.shared.corner += movementStep;
			}

			if (object->physics.shared.corner.y <= 0) {
				// potentially need to make this unconditional further down
				object->physics.shared.position.y -= object->physics.shared.corner.y;
				object->physics.shared.corner.y = 0;
				object->physics.boundary->corner = object->physics.shared.corner;
			}


		}
	}
}

void ServerGameState::updateItem() {
	// Update whatever is necesssary for item
	// This method may need to be broken down for different types
	// of item types

	auto items = this->objects.getItems();
	for (int i = 0; i < items.size(); i++) {
		auto item = items.get(i);

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
			trap->trigger();
		}
        if (trap->shouldReset(*this)) {
            std::cout << "reset\n";
            trap->reset();
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

	SpecificID floorID = this->objects.createObject(ObjectType::SolidSurface);
	SpecificID ceilingID = this->objects.createObject(ObjectType::SolidSurface);

	SolidSurface* floor = this->objects.getSolidSurface(floorID);
	SolidSurface* ceiling = this->objects.getSolidSurface(ceilingID);

	//	Set floor and ceiling's x and z dimensions equal to grid dimensions
	
	floor->shared.dimensions =
		glm::vec3(this->grid.getColumns() * this->grid.getGridCellWidth(),
			0.1,
			this->grid.getRows() * this->grid.getGridCellWidth());

	floor->physics.shared.position =
		glm::vec3(floor->shared.dimensions.x / 2, 
			-0.05,
			floor->shared.dimensions.z / 2);

	floor->physics.shared.corner = glm::vec3(0.0f, -0.1f, 0.0f);
	//floor->physics.boundary = new BoxCollider(floor->physics.shared.corner, floor->shared.dimensions);

	floor->physics.movable = false;

	ceiling->shared.dimensions = 
		glm::vec3(this->grid.getColumns() * this->grid.getGridCellWidth(),
			0.1,
			this->grid.getRows() * this->grid.getGridCellWidth());

	ceiling->physics.shared.position =
		glm::vec3(floor->shared.dimensions.x / 2, 
			MAZE_CEILING_HEIGHT + 0.05,
			floor->shared.dimensions.z / 2);

	ceiling->physics.shared.corner = glm::vec3(0.0f, -0.1f, 0.0f);

	// Not sure we would need colliders for ceiling
	//ceiling->physics.boundary = new BoxCollider(ceiling->physics.shared.corner, ceiling->shared.dimensions);

	ceiling->physics.movable = false;
	

	//	Step 6:	For each GridCell, add an object (if not empty) at the 
	//	GridCell's position.
	for (int row = 0; row < this->grid.getRows(); row++) {
		for (int col = 0; col < this->grid.getColumns(); col++) {
			GridCell* cell = this->grid.getCell(col, row);

			switch (cell->type) {
				case CellType::SpikeTrap: {
					SpecificID trapID = this->objects.createObject(ObjectType::SpikeTrap);
					SpikeTrap* trap = dynamic_cast<SpikeTrap*>(this->objects.getTrap(trapID));

                    const float HEIGHT_SHOWING = 0.5;

					trap->physics.shared.dimensions = glm::vec3(
						this->grid.getGridCellWidth(),
						MAZE_CEILING_HEIGHT,
						this->grid.getGridCellWidth()
					);
					trap->physics.shared.position = this->grid.gridCellCenterPosition(cell) + 
                        glm::vec3(0, MAZE_CEILING_HEIGHT + (MAZE_CEILING_HEIGHT / 2), 0) - HEIGHT_SHOWING;
					trap->physics.shared.corner = 
						glm::vec3(cell->x * this->grid.getGridCellWidth(),
							MAZE_CEILING_HEIGHT - HEIGHT_SHOWING, 
							cell->y * this->grid.getGridCellWidth());
					trap->physics.boundary = new BoxCollider(trap->physics.shared.corner, trap->physics.shared.dimensions);
					break;
				}
				case CellType::Enemy: {
					SpecificID enemyID = this->objects.createObject(ObjectType::Enemy);

					Enemy* enemy = this->objects.getEnemy(enemyID);
					enemy->physics.movable = false;
					enemy->physics.shared.position = this->grid.gridCellCenterPosition(cell);
					break;
				}
				case CellType::Wall: {
					//	Create a new Wall object
					SpecificID wallID = 
						this->objects.createObject(ObjectType::SolidSurface);

					//	TODO: Shouldn't this use the typeID? Change
					//	createObject() to return the typeID of an object and
					//	add specific object getters based on their types.
					SolidSurface* wall = this->objects.getSolidSurface(wallID);

					wall->shared.dimensions =
						glm::vec3(this->grid.getGridCellWidth(),
							MAZE_CEILING_HEIGHT,
							this->grid.getGridCellWidth());

					wall->physics.shared.position =
						this->grid.gridCellCenterPosition(cell)
						+ glm::vec3(0, MAZE_CEILING_HEIGHT / 2, 0);
						/*glm::vec3((0.5 + cell->x) * this->grid.getGridCellWidth(),
							MAZE_CEILING_HEIGHT / 2,
							(0.5 + cell->y) * this->grid.getGridCellWidth());*/
					
					wall->physics.shared.corner = 
						glm::vec3(cell->x * this->grid.getGridCellWidth(),
							0.0f, 
							cell->y * this->grid.getGridCellWidth());
					wall->physics.boundary = new BoxCollider(wall->physics.shared.corner, wall->shared.dimensions);

					wall->physics.movable = false;

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