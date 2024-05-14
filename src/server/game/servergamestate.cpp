#include "server/game/servergamestate.hpp"
#include "server/game/spiketrap.hpp"
#include "server/game/fireballtrap.hpp"
#include "server/game/floorspike.hpp"
#include "server/game/fakewall.hpp"
#include "server/game/projectile.hpp"
#include "server/game/arrowtrap.hpp"
#include "server/game/potion.hpp"
#include "server/game/constants.hpp"
#include "shared/game/sharedgamestate.hpp"
#include "shared/utilities/root_path.hpp"
#include "shared/utilities/time.hpp"
#include "shared/network/constants.hpp"

#include <fstream>

/*	Constructors and Destructors	*/

ServerGameState::ServerGameState() : ServerGameState(getDefaultConfig()) {}

ServerGameState::ServerGameState(GameConfig config) {
	this->phase = GamePhase::LOBBY;
	this->timestep = FIRST_TIMESTEP;
	this->timestep_length = config.game.timestep_length_ms;
	this->lobby.max_players = config.server.max_players;
	this->lobby.name = config.server.lobby_name;
	this->is_dungeon_master = config.server.is_dungeon_master;

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
std::vector<SharedGameState> ServerGameState::generateSharedGameState(bool send_all) {
	std::vector<SharedGameState> partial_updates;
	auto all_objects = this->objects.toShared();

	auto getUpdateTemplate = [this]() {
		SharedGameState curr_update;
		curr_update.timestep = this->timestep;
		curr_update.timestep_length = this->timestep_length;
		curr_update.lobby = this->lobby;
		curr_update.phase = this->phase;
		return curr_update;
	};

	if (send_all) {
		for (int i = 0; i < all_objects.size(); i += OBJECTS_PER_UPDATE) {
			SharedGameState curr_update = getUpdateTemplate();

			for (int j = 0; j < OBJECTS_PER_UPDATE && i + j < all_objects.size(); j++) {
				EntityID curr_id = i + j;
				curr_update.objects.insert({curr_id, all_objects.at(curr_id)});
			}

			partial_updates.push_back(curr_update);
		}
	} else {
		SharedGameState curr_update = getUpdateTemplate();

		int num_in_curr_update = 0;
		for (EntityID id : this->updated_entities) {
			curr_update.objects.insert({id, all_objects.at(id)});
			num_in_curr_update++;

			if (num_in_curr_update >= OBJECTS_PER_UPDATE) {
				partial_updates.push_back(curr_update);
				curr_update = getUpdateTemplate();
				num_in_curr_update = 0;
			}
		}

		if (num_in_curr_update > 0) {
			partial_updates.push_back(curr_update);
		}

		// wipe updated entities list
		std::unordered_set<EntityID> empty;
		std::swap(this->updated_entities, empty);
	}

	return partial_updates;
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
            obj = this->objects.getObject(changeFacingEvent.entity_to_change_face);
            obj->physics.shared.facing = changeFacingEvent.facing;
			this->updated_entities.insert({obj->globalID});
            break;
		}
	
		case EventType::StartAction: {
			auto startAction = boost::get<StartActionEvent>(event.data);
			obj = this->objects.getObject(startAction.entity_to_act);
			this->updated_entities.insert({obj->globalID});
			
			//switch case for action (currently using keys)
			switch (startAction.action) {
			case ActionType::MoveCam: {
				obj->physics.velocity.x = (startAction.movement * PLAYER_SPEED).x;
				obj->physics.velocity.z = (startAction.movement * PLAYER_SPEED).z;
				break;
			}
			case ActionType::Jump: {
				if (!obj->physics.feels_gravity || obj->physics.velocity.y != 0) { break; }
				obj->physics.velocity.y += (startAction.movement * JUMP_SPEED / 2.0f).y;
				break;
			}
			case ActionType::Sprint: {
				obj->physics.velocityMultiplier = glm::vec3(1.5f, 1.1f, 1.5f);
				break;
			}
			case ActionType::Zoom: { // only for DM
				DungeonMaster * dm = this->objects.getDM();

				if ((dm->physics.shared.corner.y + startAction.movement.y >= 10.0f) && (dm->physics.shared.corner.y + startAction.movement.y <= 50.0f))
					dm->physics.shared.corner += startAction.movement;

				break;
			}
			default: {}
			}
			break;
		}

		case EventType::StopAction: {
			auto stopAction = boost::get<StopActionEvent>(event.data);
			obj = this->objects.getObject(stopAction.entity_to_act);
			this->updated_entities.insert({obj->globalID});
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
			obj = this->objects.getObject(moveRelativeEvent.entity_to_move);
			obj->physics.velocity += moveRelativeEvent.movement;
			this->updated_entities.insert(obj->globalID);
			break;

		}
		case EventType::SelectItem:
		{
			auto selectItemEvent = boost::get<SelectItemEvent>(event.data);
			player->sharedInventory.selected = selectItemEvent.itemNum;
			this->updated_entities.insert(player->globalID);
			break;
		}
		case EventType::UseItem:
		{
			auto useItemEvent = boost::get<UseItemEvent>(event.data);
			int itemSelected = player->sharedInventory.selected;
			this->updated_entities.insert(player->globalID);

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
			this->updated_entities.insert(player->globalID);

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
	//	Update all movable objects' positions

	//	Iterate through all objects in the ServerGameState and update their
	//	positions and velocities if they are movable.

	//	If objects move too fast, split their movement into NUM_INCREMENTAL_STEPS
	const int NUM_INCREMENTAL_STEPS = 6;

	//	This is the threshold that determines whether we need to use incremental
	//	steps. If the magnitude of the movementStep vector is greater than this
	//	value, then we'll split the movementStep into multiple incremental steps
	const float SINGLE_MOVE_THRESHOLD = 0.33f;

	//	This ratio is the reciprocal of NUM_INCREMENTAL_STEPS
	const float INCREMENTAL_MOVE_RATIO = 1.0f / NUM_INCREMENTAL_STEPS;

	//	Iterate through all game objects
	SmartVector<Object*> gameObjects = this->objects.getObjects();
	for (int i = 0; i < gameObjects.size(); i++) {
		//	Get iterating game object
		Object* object = gameObjects.get(i);

		//	If the object is a nullptr or isn't movable, skip
		if (object == nullptr || !(object->physics.movable))
			continue;

		//	Object is movable - for now, add to updated entities set
		this->updated_entities.insert(object->globalID);

		//	Object is movable - compute total movement step
		glm::vec3 totalMovementStep = 
			object->physics.velocity * object->physics.velocityMultiplier;
		totalMovementStep.x *= object->physics.nauseous;
		totalMovementStep.z *= object->physics.nauseous;

		// handle edge case Dungeon Master
		if (object->type == ObjectType::DungeonMaster) {
			object->physics.shared.corner += totalMovementStep;

			// Rows * grid cell width = z length and columns * grid cell width = x length
			// check z length
			bool zInBounds = (object->physics.shared.corner.z <= this->grid.getRows() * this->grid.grid_cell_width) && (object->physics.shared.corner.z >= 0);

			// check x length
			bool xInBounds = (object->physics.shared.corner.x <= this->grid.getColumns() * this->grid.grid_cell_width) && (object->physics.shared.corner.x >= 0);

			if (!zInBounds) {
				object->physics.shared.corner.z -= totalMovementStep.z;
			}

			if (!xInBounds) {
				object->physics.shared.corner.x -= totalMovementStep.x;
			}

			continue;
		}

		if (object->physics.collider == Collider::None) {
			this->objects.moveObject(object, object->physics.shared.corner + totalMovementStep);
			continue;
		}

		glm::vec3 movementStep;
		int numSteps = 0;
		if (glm::length(totalMovementStep) > SINGLE_MOVE_THRESHOLD) {
			movementStep = INCREMENTAL_MOVE_RATIO * totalMovementStep;
		} else {
			movementStep = totalMovementStep;
			numSteps = NUM_INCREMENTAL_STEPS - 1;
		}

		bool collided = false;
		bool collidedX = false;
		bool collidedZ = false;

		//	Object's current position (before current movementStep)
		glm::vec3 currentPosition = object->physics.shared.corner;

		//	Perform collision detection + movement update for each incremental
		//	step (or this loop only iterates once if incremental steps are not
		//	used)
		while (numSteps < NUM_INCREMENTAL_STEPS) {
			numSteps++;

			//	Move object to new position and check whether a collision has
			//	occurred
			collided = this->hasObjectCollided(object, 
				currentPosition + movementStep);

			//	If a collision has occurred, repeat collision checking for
			//	movement if object's position is only updated in the x or z
			//	axes.
			if (collided) {
				//	Test for collision when object only moves by movementStep's
				//	x component
				if (!collidedX) {
					collidedX = this->hasObjectCollided(object,
						glm::vec3(
							currentPosition.x + movementStep.x,
							currentPosition.y,
							currentPosition.z
						));
				}
				
				//	Test for collision when object only moves by movementStep's
				//	z component
				if (!collidedZ) {
					collidedZ = this->hasObjectCollided(object,
						glm::vec3(
							currentPosition.x,
							currentPosition.y,
							currentPosition.z + movementStep.z
						));
				}
			}

			//	Update object's movement

			//	Horizontal movement
			if (collidedX) {
				movementStep.x = 0;
			}
			if (collidedZ) {
				movementStep.z = 0;
			}

			//	Move object to the specified movement step
			this->objects.moveObject(object, currentPosition + movementStep);

			if (collidedX && collidedZ) {
				//	Object doesn't move at all - can skip any additional
				//	steps
				break;
			}

			//	Update current position vector
			currentPosition = object->physics.shared.corner;
		}

		//	Vertical movement
		//	Clamp object to floor if corner's y position is lower than the floor
		if (object->physics.shared.corner.y < 0) {
			object->physics.shared.corner.y = 0;
		}

		//	Update object's gravity velocity if the object is in the air or
		//	has just landed
		// update gravity factor
		if (object->physics.feels_gravity) {
			if ((object->physics.shared.corner).y > 0) {
				object->physics.velocity.y -= GRAVITY;
			}
			else {
				object->physics.velocity.y = 0.0f;
			}
		}
	}

	//	Handle collision resolution effects
	//	NOTE - if collision resolution can change an object's position, behavior
	//	is undefined! (e.g., an object can move into another object but
	//	collision detection is not performed!)
	//	Iterate through set of collided objects
	for (std::pair<Object*, Object*> objects : this->collidedObjects) {
		objects.first->doCollision(objects.second, *this);
		objects.second->doCollision(objects.first, *this);

		//	Add both collided objects to updated entities set
		this->updated_entities.insert(objects.first->globalID);
		this->updated_entities.insert(objects.second->globalID);
	}

	//	Clear set of collided objects for this timestep
	this->collidedObjects.clear();
}

bool ServerGameState::hasObjectCollided(Object* object, glm::vec3 newCornerPosition) {
	//	Move object to the given corner position then test whether a collision 
	//	occurs at that position
	this->objects.moveObject(object, newCornerPosition);

	//	Check whether a collision has occurred in object's new corner position
	//	Iterate through the object's occupied grid cells
	for (glm::ivec2 cellPos : object->gridCellPositions) {
		//	Get vector of pointers of all objects that occupy the iterating
		//	GridCell position
		std::vector<Object*>& objectsInCell =
			this->objects.cellToObjects.at(cellPos);

		//	Iterate through all objects in this GridCell and check whether the
		//	current object collides with any of them
		for (Object* otherObj : objectsInCell) {
			//	Skip other object if it's the current object or if the object
			//	doesn't have a collider
			if (object->globalID == otherObj->globalID
				|| otherObj->physics.collider == Collider::None) {
				continue;
			}

			//	Detect whether a collision has occurred
			if (detectCollision(object->physics, otherObj->physics))
			{
				//	Add object pair to set of collided objects
				this->collidedObjects.insert({ object, otherObj });

				//	Handle collision response between the two objects
				/*object->doCollision(otherObj, this);
				otherObj->doCollision(object, this);*/

				//	Exception - if the other object is a floor spike trap,
				//	perform collision handling but do not return true as the
				//	trap doesn't affect the movement of the object it hits
				if (otherObj->type == ObjectType::FloorSpike) {
					continue;
				}

				return true;
			}
		}
	}

	return false;
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
					this->updated_entities.insert(pot->globalID);
				}
			}
		}

	}
}

//void ServerGameState::useItem() {
//	// Update whatever is necesssary for item
//	// This method may need to be broken down for different types
//	// of item types
//
//	SmartVector<Item*> items = this->objects.getItems();
//	for (int i = 0; i < items.size(); i++) {
//		const Item* item = items.get(i);
//
//		if (item == nullptr)
//			continue;
//	}
//}

void ServerGameState::doObjectTicks() {
	auto objects = this->objects.getObjects();
	for (int o = 0; o < objects.size(); o++) {
		auto obj = objects.get(o);
		if (obj == nullptr) continue;

		if (obj->doTick(*this)) {
			this->updated_entities.insert(obj->globalID);
		}
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
			this->updated_entities.insert(trap->globalID);
		}
        if (trap->shouldReset(*this)) {
            trap->reset(*this);
			this->updated_entities.insert(trap->globalID);
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
			this->updated_entities.insert(player->globalID);
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
				this->updated_entities.insert(player->globalID);
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
		this->updated_entities.insert(id);
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
				case CellType::FireballTrap: {
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
					this->objects.createObject(new FireballTrap(corner, dimensions));
					break;
				}
				case CellType::HealthPotion: {
					glm::vec3 dimensions(1.0f);

					glm::vec3 corner(cell->x * Grid::grid_cell_width + 1,
							0,
							cell->y * Grid::grid_cell_width + 1);

					this->objects.createObject(new Potion(corner, dimensions, PotionType::Health));
					break;
				}
				case CellType::NauseaPotion: {
					glm::vec3 dimensions(1.0f);

					glm::vec3 corner(cell->x* Grid::grid_cell_width + 1,
						0,
						cell->y* Grid::grid_cell_width + 1);

					this->objects.createObject(new Potion(corner, dimensions, PotionType::Nausea));
					break;
				}
				case CellType::InvisibilityPotion: {
					glm::vec3 dimensions(1.0f);

					glm::vec3 corner(cell->x* Grid::grid_cell_width + 1,
						0,
						cell->y* Grid::grid_cell_width + 1);

					this->objects.createObject(new Potion(corner, dimensions, PotionType::Invisibility));
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
						Grid::grid_cell_width,
						MAZE_CEILING_HEIGHT,
						Grid::grid_cell_width
					);
					glm::vec3 corner(
						cell->x * Grid::grid_cell_width,
						0.0f, 
						cell->y * Grid::grid_cell_width
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