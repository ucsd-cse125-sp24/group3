#include "server/game/servergamestate.hpp"
#include "shared/game/sharedgamestate.hpp"
#include "server/game/spiketrap.hpp"
#include "server/game/fireballtrap.hpp"
#include "server/game/floorspike.hpp"
#include "server/game/fakewall.hpp"
#include "server/game/projectile.hpp"
#include "server/game/arrowtrap.hpp"
#include "server/game/potion.hpp"
#include "shared/utilities/root_path.hpp"
#include "server/game/constants.hpp"
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
		case EventType::SelectItem:
		{
			auto selectItemEvent = boost::get<SelectItemEvent>(event.data);
			player->sharedInventory.selected = selectItemEvent.itemNum;
			break;
		}
		case EventType::UseItem:
		{
			auto useItemEvent = boost::get<UseItemEvent>(event.data);
			int itemSelected = player->sharedInventory.selected;

			if (player->inventory.find(itemSelected) != player->inventory.end()) {
				Item* item = this->objects.getItem(player->inventory.at(itemSelected));
				item->useItem(player, *this);
				player->inventory.erase(itemSelected);
				player->sharedInventory.inventory.erase(itemSelected);
				//TODO : should also remove item afterwards
			}
			break;
		}
		case EventType::DropItem:
		{
			auto dropItemEvent = boost::get<DropItemEvent>(event.data);
			int itemSelected = player->sharedInventory.selected;
			if (player->inventory.find(itemSelected) != player->inventory.end()) {
				Item* item = this->objects.getItem(player->inventory.at(itemSelected));
				item->iteminfo.held = false;
				item->physics.collider = Collider::Box;
				item->physics.shared.corner = (player->physics.shared.corner + (player->physics.shared.facing * 4.0f)) * glm::vec3(1.0f, 0.0f, 1.0f);
				player->inventory.erase(itemSelected);
				player->sharedInventory.inventory.erase(itemSelected);
			}
			break;
		}

		// default:
		//     std::cerr << "Unimplemented EventType (" << event.type << ") received" << std::endl;
        }
    }

	//	TODO: fill update() method with updating object movement
	doObjectTicks();
	updateMovement();
	updateItems();
	updateTraps();
	handleDeaths();
	handleRespawns();
	deleteEntities();
	
	//	Increment timestep
	this->timestep++;
}

void ServerGameState::markForDeletion(EntityID id) {
	this->entities_to_delete.insert(id);
}

void ServerGameState::updateMovement() {
	//	Update all movable objects' movement

	//	Iterate through all objects in the ServerGameState and update their
	//	positions and velocities if they are movable.

	// If objects are moving too fast, we split their movement into NUM_INCREMENTAL_STEPS smaller steps
	const int NUM_INCREMENTAL_STEPS = 6;
	// This is the threshold that determines if we need to do incremental steps for the movement
	// if the magnitude of movementStep is greater than this value, then we do incremental steps for the movement
	const float SINGLE_MOVE_THRESHOLD = 0.33f;

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
			} else {
				movementStep = totalMovementStep;
				numSteps = NUM_INCREMENTAL_STEPS - 1;
			}

			while (numSteps < NUM_INCREMENTAL_STEPS) {
				numSteps++;
				// Run collision detection movement if it has a collider
				if (object->physics.collider != Collider::None) {
					object->physics.shared.corner += movementStep;

					// TODO : for possible addition for smooth collision detection, but higher computation
					// 1) when moving collider, seperate the movement into 4 steps ex:(object->physics.velocity * object->physics.acceleration) / 4
					//    Then, take the most steps possible (mario 64 handles it like this)
					// 2) Using raycasting

					for (int j = 0; j < gameObjects.size(); j++) {
						if (i == j) { continue; }
						Object* otherObj = gameObjects.get(j);
						if (otherObj == nullptr) continue;

						if (otherObj->physics.collider == Collider::None) { continue; }

						if (detectCollision(object->physics, otherObj->physics)) {
							
							if (otherObj->type == ObjectType::FloorSpike) {
								object->doCollision(otherObj, *this);
								otherObj->doCollision(object, *this);
								continue;
							}
							if (otherObj->type == ObjectType::Potion) {
								otherObj->doCollision(object, *this);
								continue;
							}
							

							collided = true;

							// Check x-axis collision
							object->physics.shared.corner.z -= movementStep.z;
							if (detectCollision(object->physics, otherObj->physics)) {
								collidedX = true;
							}

							// Check z-axis collision
							object->physics.shared.corner.z += movementStep.z;
							object->physics.shared.corner.x -= movementStep.x;
							if (detectCollision(object->physics, otherObj->physics)) {
								collidedZ = true;
							}
							object->physics.shared.corner.x += movementStep.x;

							object->doCollision(otherObj, *this);
							otherObj->doCollision(object, *this);
						}
					}

					if (collidedX) {
						object->physics.shared.corner.x -= movementStep.x;
					}

					if (collidedZ) {
						object->physics.shared.corner.z -= movementStep.z;
					}
				}
				else {
					object->physics.shared.corner += movementStep;
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

void ServerGameState::updateItems() {
	auto items = this->objects.getItems();
	for (int i = 0; i < items.size(); i++) {
		auto item = items.get(i);
		if (item == nullptr) { continue; }

		if (item->type == ObjectType::Potion) {
			Potion* pot = dynamic_cast<Potion*>(item);
			if (pot->iteminfo.used) {
				if (pot->timeOut()) {
					pot->revertEffect(*this);
				}
			}
		}
		
	}
}

void ServerGameState::doObjectTicks() {
	auto objects = this->objects.getObjects();
	for (int o = 0; o < objects.size(); o++) {
		auto obj = objects.get(o);
		if (obj == nullptr) continue;

		obj->doTick(*this);
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
				player->stats.health.increase(player->stats.health.max());
			}
		}
	}
}

void ServerGameState::deleteEntities() {
	for (EntityID id : this->entities_to_delete) {
		this->objects.removeObject(id);
	}

	std::unordered_set<EntityID> empty;
	std::swap(this->entities_to_delete, empty);
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
		glm::vec3(this->grid.getColumns() * this->grid.getGridCellWidth(), 0.1,
			this->grid.getRows() * this->grid.getGridCellWidth())
	));
	// Create Ceiling
	this->objects.createObject(new SolidSurface(false, Collider::None, SurfaceType::Ceiling, 
		glm::vec3(0.0f, MAZE_CEILING_HEIGHT, 0.0f),
		glm::vec3(this->grid.getColumns() * this->grid.getGridCellWidth(), 0.1,
			this->grid.getRows() * this->grid.getGridCellWidth())
	));

	//	Step 6:	For each GridCell, add an object (if not empty) at the 
	//	GridCell's position.
	for (int row = 0; row < this->grid.getRows(); row++) {
		for (int col = 0; col < this->grid.getColumns(); col++) {
			GridCell* cell = this->grid.getCell(col, row);

			switch (cell->type) {
				case CellType::FireballTrap: {
					glm::vec3 dimensions(
						this->grid.getGridCellWidth() / 2,
						0.5f,
						this->grid.getGridCellWidth() / 2
					);
					glm::vec3 corner(
						cell->x * this->grid.getGridCellWidth(),
						1.0f,
						cell->y * this->grid.getGridCellWidth()
					);
					this->objects.createObject(new FireballTrap(corner, dimensions));
					break;
				}
				case CellType::HealthPotion: {
					glm::vec3 dimensions(1.0f);

					glm::vec3 corner(cell->x * this->grid.getGridCellWidth() + 1,
							0,
							cell->y * this->grid.getGridCellWidth() + 1);

					this->objects.createObject(new Potion(corner, dimensions, PotionType::Health));
					break;
				}
				case CellType::NauseaPotion: {
					glm::vec3 dimensions(1.0f);

					glm::vec3 corner(cell->x* this->grid.getGridCellWidth() + 1,
						0,
						cell->y* this->grid.getGridCellWidth() + 1);

					this->objects.createObject(new Potion(corner, dimensions, PotionType::Nausea));
					break;
				}
				case CellType::InvisibilityPotion: {
					glm::vec3 dimensions(1.0f);

					glm::vec3 corner(cell->x* this->grid.getGridCellWidth() + 1,
						0,
						cell->y* this->grid.getGridCellWidth() + 1);

					this->objects.createObject(new Potion(corner, dimensions, PotionType::Invisibility));
					break;
				}
				case CellType::SpikeTrap: {
                    const float HEIGHT_SHOWING = 0.5;
					glm::vec3 dimensions(
						this->grid.getGridCellWidth(),
						MAZE_CEILING_HEIGHT,
						this->grid.getGridCellWidth()
					);
					glm::vec3 corner(
						cell->x * this->grid.getGridCellWidth(),
						MAZE_CEILING_HEIGHT - HEIGHT_SHOWING, 
						cell->y * this->grid.getGridCellWidth()
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
						this->grid.getGridCellWidth(),
						MAZE_CEILING_HEIGHT,
						this->grid.getGridCellWidth()
					);
					glm::vec3 corner(
						cell->x * this->grid.getGridCellWidth(),
						0.0f, 
						cell->y * this->grid.getGridCellWidth()
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
						cell->x * this->grid.getGridCellWidth(),
						0.0f, 
						cell->y * this->grid.getGridCellWidth()
					);

					FloorSpike::Orientation orientation;
					if (cell->type == CellType::FloorSpikeFull) {
						orientation = FloorSpike::Orientation::Full;
					} else if (cell->type == CellType::FloorSpikeHorizontal) {
						orientation = FloorSpike::Orientation::Horizontal;
						corner.z += this->grid.getGridCellWidth() * 0.25f;
					} else {
						orientation = FloorSpike::Orientation::Vertical;
						corner.x += this->grid.getGridCellWidth() * 0.25f;
					}

					this->objects.createObject(new FloorSpike(corner, orientation, this->grid.getGridCellWidth()));
					break;
				}

				case CellType::ArrowTrapDown:
				case CellType::ArrowTrapLeft:
				case CellType::ArrowTrapRight:
				case CellType::ArrowTrapUp: {
					ArrowTrap::Direction dir;
					if (cell->type == CellType::ArrowTrapDown) {
						dir = ArrowTrap::Direction::DOWN;
					} else if (cell->type == CellType::ArrowTrapUp) {
						dir = ArrowTrap::Direction::UP;
					} else if (cell->type == CellType::ArrowTrapLeft) {
						dir = ArrowTrap::Direction::LEFT;
					} else {
						dir = ArrowTrap::Direction::RIGHT;
					}

					glm::vec3 dimensions(
						this->grid.getGridCellWidth(),
						MAZE_CEILING_HEIGHT,
						this->grid.getGridCellWidth()
					);
					glm::vec3 corner(
						cell->x * this->grid.getGridCellWidth(),
						0.0f, 
						cell->y * this->grid.getGridCellWidth()
					);

					this->objects.createObject(new ArrowTrap(corner, dimensions, dir));
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