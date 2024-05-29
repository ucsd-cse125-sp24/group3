#include "server/game/servergamestate.hpp"
#include "server/game/gridcell.hpp"
#include "server/game/object.hpp"
#include "server/game/torchlight.hpp"
#include "shared/game/sharedgamestate.hpp"
#include "server/game/spiketrap.hpp"
#include "server/game/fireballtrap.hpp"
#include "server/game/slime.hpp"
#include "server/game/floorspike.hpp"
#include "server/game/fakewall.hpp"
#include "server/game/teleportertrap.hpp"
#include "server/game/projectile.hpp"
#include "server/game/arrowtrap.hpp"
#include "server/game/potion.hpp"
#include "server/game/constants.hpp"
#include "server/game/exit.hpp"
#include "server/game/orb.hpp"
#include "server/game/weapon.hpp"
#include "server/game/weaponcollider.hpp"
#include "server/game/spawner.hpp"

#include "shared/game/celltype.hpp"
#include "shared/game/sharedgamestate.hpp"
#include "shared/audio/constants.hpp"
#include "shared/audio/utilities.hpp"
#include "shared/game/sharedobject.hpp"
#include "shared/utilities/root_path.hpp"
#include "shared/utilities/time.hpp"
#include "shared/network/constants.hpp"
#include "server/game/grid.hpp"
#include "shared/utilities/rng.hpp"
#include "server/game/mazegenerator.hpp"

#include <fstream>

/*	Constructors and Destructors	*/

ServerGameState::ServerGameState() : ServerGameState(getDefaultConfig()) {}

ServerGameState::ServerGameState(GameConfig config) {
	this->phase = GamePhase::LOBBY;
	this->timestep = FIRST_TIMESTEP;
	this->lobby = Lobby(config.server.max_players);
	this->lobby.max_players = config.server.max_players;
	this->lobby.name = config.server.lobby_name;

	this->maps_directory = config.game.maze.directory;
	this->maze_file = config.game.maze.maze_file;

	//	Initialize game instance match phase data
	//	Match begins in MazeExploration phase (no timer)
	this->matchPhase = MatchPhase::MazeExploration;
	this->timesteps_left = TIME_LIMIT_MS / TIMESTEP_LEN;
	//	Player victory is by default false (need to collide with an open exit
	//	while holding the Orb to win, whereas DM wins on time limit expiration)
	this->playerVictory = false;

	//	No player died yet
	this->numPlayerDeaths = 0;

	this->currentGhostTrap = nullptr;
	this->spawner = std::make_unique<Spawner>();
	this->spawner->spawnDummy(*this);

    MazeGenerator generator(config);
    int attempts = 1;
    auto grid = generator.generate();
    if (!grid.has_value() || std::abs(grid->getColumns()) > MAX_MAZE_COLUMNS) {
		if (grid->getColumns() > MAX_MAZE_COLUMNS) {
			std::cerr << "SUS! The maze has " << grid->getColumns() << "Columns!\n"
				<< "I dont feel like fixing this, so we are going to try again!\n";
		}
		// failed so try again
		generator = MazeGenerator(config);
		grid = generator.generate();
        attempts++;
    }

	if (config.game.maze.procedural) {
		std::cout << "Took " << attempts << " attempts to generate a full procedural maze\n";
		std::string filename = std::to_string(getMsSinceEpoch()) + ".maze";
		auto path = getRepoRoot() / config.game.maze.directory / "generated" / filename;
		std::cout << "Saving procedural maze to " << path << std::endl;
		grid->writeToFile(path.string());
	}

	//	Load maze (Note: This only happens in THIS constructor! All other
	//	ServerGameState constructors MUST call this constructor to load the
	//	maze environment from a file)
	this->loadMaze(*grid);
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
		curr_update.lobby = this->lobby;
		curr_update.phase = this->phase;
		curr_update.matchPhase = this->matchPhase;
		curr_update.timesteps_left = this->timesteps_left;
		curr_update.playerVictory = this->playerVictory;
		curr_update.numPlayerDeaths = this->numPlayerDeaths;
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

		//	Make sure that SharedGameState updates are sent while the server is in the
		//	Lobby phase (to ensure players can see other players lobby status updates)
		if (num_in_curr_update > 0 || this->getPhase() == GamePhase::LOBBY) {
			partial_updates.push_back(curr_update);
		}

		// wipe updated entities list
		std::unordered_set<EntityID> empty;
		std::swap(this->updated_entities, empty);
	}

	//	DEBUG
	/*if (partial_updates.size() > 0) {
		std::cout << "Number of partial updates: " << std::to_string(partial_updates.size()) << std::endl;
		std::cout << "Partial update's lobby (in server):" << std::endl;
		std::cout << partial_updates[0].lobby.to_string() << std::endl;
	}*/

	return partial_updates;
}

SoundTable& ServerGameState::soundTable() {
	return this->sound_table;
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
			this->updated_entities.insert({ obj->globalID });
			break;
		}

		case EventType::StartAction: {
			auto startAction = boost::get<StartActionEvent>(event.data);
			obj = this->objects.getObject(startAction.entity_to_act);
			this->updated_entities.insert({ obj->globalID });

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
				this->sound_table.addNewSoundSource(SoundSource(
					ServerSFX::PlayerJump,
					obj->physics.shared.corner,
					DEFAULT_VOLUME,
					MEDIUM_DIST,
					MEDIUM_ATTEN
				));
				break;
			}
			case ActionType::Sprint: {
				if (obj->type == ObjectType::DungeonMaster) {
					obj->physics.velocityMultiplier = glm::vec3(5.0f, 1.1f, 5.0f);
				}
				else {
					obj->physics.velocityMultiplier = glm::vec3(1.5f, 1.1f, 1.5f);
				}
				break;
			}
			case ActionType::Zoom: { // only for DM
				DungeonMaster * dm = this->objects.getDM();

				if ((dm->physics.shared.corner.y + startAction.movement.y >= 10.0f) && (dm->physics.shared.corner.y + startAction.movement.y <= 100.0f))
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
			this->updated_entities.insert({ obj->globalID });
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
			obj = this->objects.getObject(selectItemEvent.playerEID);

			if (obj == nullptr) {
				std::cerr << "Certified Bruh Moment... " << __LINE__ << "\n";
				std::exit(1);
			}

			if (obj->type == ObjectType::DungeonMaster) {
				DungeonMaster* dm = this->objects.getDM();

				if (dm->sharedTrapInventory.selected + selectItemEvent.itemNum == 0)
					dm->sharedTrapInventory.selected = TRAP_INVENTORY_SIZE;
				else if (dm->sharedTrapInventory.selected + selectItemEvent.itemNum == TRAP_INVENTORY_SIZE + 1)
					dm->sharedTrapInventory.selected = 1;
				else
					dm->sharedTrapInventory.selected = dm->sharedTrapInventory.selected + selectItemEvent.itemNum;

				this->updated_entities.insert(dm->globalID);
			}
			else {
				if (player->sharedInventory.selected + selectItemEvent.itemNum == 0)
					player->sharedInventory.selected = INVENTORY_SIZE;
				else if (player->sharedInventory.selected + selectItemEvent.itemNum == INVENTORY_SIZE + 1)
					player->sharedInventory.selected = 1;
				else
					player->sharedInventory.selected = player->sharedInventory.selected + selectItemEvent.itemNum;

				this->updated_entities.insert(player->globalID);
			}
			break;
		}
		case EventType::UseItem:
		{
			auto useItemEvent = boost::get<UseItemEvent>(event.data);
			int itemSelected = player->sharedInventory.selected - 1;

			if (player->inventory[itemSelected] != -1) {
				Item* item = this->objects.getItem(player->inventory[itemSelected]);
				item->useItem(player, *this, itemSelected);

				this->updated_entities.insert(player->globalID);
				this->updated_entities.insert(item->globalID);
			}
			break;
		}
		case EventType::DropItem:
		{
			auto dropItemEvent = boost::get<DropItemEvent>(event.data);
			int itemSelected = player->sharedInventory.selected - 1;

			if (player->inventory[itemSelected] != -1) {
				Item* item = this->objects.getItem(player->inventory[itemSelected]);
				item->dropItem(player, *this, itemSelected, 3.0f);

				this->updated_entities.insert(player->globalID);
				this->updated_entities.insert(item->globalID);
			}
			break;
		}
		case EventType::TrapPlacement: 
		{
			auto trapPlacementEvent = boost::get<TrapPlacementEvent>(event.data);

			Grid& currGrid = this->getGrid();

			float cellWidth = currGrid.grid_cell_width;

			DungeonMaster* dm = this->objects.getDM();

			glm::vec3 dir = glm::normalize(trapPlacementEvent.world_pos-dm->physics.shared.corner);

			trapPlacementEvent.world_pos += (dir*(float)DM_Z_DISCOUNT);

			glm::ivec2 gridCellPos = currGrid.getGridCellFromPosition(trapPlacementEvent.world_pos);

			GridCell* cell = currGrid.getCell(gridCellPos.x, gridCellPos.y);

			if (cell == nullptr)
				break;

			this->updated_entities.insert(dm->globalID);

			// mark previous ghost trap for deletion, if exists
			if (this->currentGhostTrap != nullptr) {
				markForDeletion(this->currentGhostTrap->globalID);
				this->currentGhostTrap = nullptr; // reset ghost trap variable
			}

			if (trapPlacementEvent.hover) {
				// only for traps, not lightning
				if (trapPlacementEvent.cell == CellType::Lightning) {
					break;
				}

				Trap* trap = placeTrapInCell(cell, trapPlacementEvent.cell);

				if (trap == nullptr)
					break;

				this->currentGhostTrap = trap;

				this->currentGhostTrap->setIsDMTrapHover(true);

				this->updated_entities.insert(trap->globalID);
			}
			else if(trapPlacementEvent.place) {
				auto curr_time = std::chrono::system_clock::now();

				int trapsPlaced = dm->getPlacedTraps();

				if (trapsPlaced == MAX_TRAPS) {
					break;
				}

				auto it = dm->sharedTrapInventory.trapsInCooldown.find(trapPlacementEvent.cell);

				// in cooldown map sadly
				if (it != dm->sharedTrapInventory.trapsInCooldown.end()) {
					break;
				}
				
				if(trapPlacementEvent.cell == CellType::Lightning){
					Weapon* lightning = dm->lightning;
					glm::vec3 corner(
						cell->x * Grid::grid_cell_width,
						0.0f,
						cell->y * Grid::grid_cell_width
					);

					lightning->useLightning(dm, *this, corner);

					dm->sharedTrapInventory.trapsInCooldown[trapPlacementEvent.cell] = std::chrono::system_clock::to_time_t(curr_time);
					break;
				}

				Trap* trap = placeTrapInCell(cell, trapPlacementEvent.cell);

				if (trap == nullptr) { 
					break;
				}

				trap->setIsDMTrap(true);
				trap->setExpiration(curr_time + std::chrono::seconds(10));

				this->updated_entities.insert(trap->globalID);

				dm->sharedTrapInventory.trapsInCooldown[trapPlacementEvent.cell] = std::chrono::system_clock::to_time_t(curr_time);

				dm->setPlacedTraps(trapsPlaced + 1);

				dm->sharedTrapInventory.trapsPlaced = trapsPlaced + 1;
			}
			break;
		}

		// default:
		//     std::cerr << "Unimplemented EventType (" << event.type << ") received" << std::endl;
		}
	}


	//	TODO: fill update() method with updating object movement
	doProjectileTicks();
    doTorchlightTicks();
	updateMovement();
	updateAttacks();
	updateEnemies();
	updateItems();
	updateTraps();
	handleDeaths();
	handleRespawns();
	deleteEntities();
	spawnEnemies();
	handleTickVelocity();
	tickStatuses();
	
	//	Increment timestep
	this->timestep++;

	//	Countdown timer if the Orb has been picked up by a Player and the match
	//	phase is now RelayRace
	if (this->matchPhase == MatchPhase::RelayRace) {
		this->timesteps_left--;

		if (this->timesteps_left == 0) {
			//	Dungeon Master won on time limit expiration
			this->phase = GamePhase::RESULTS;
		}
	}
}

void ServerGameState::markForDeletion(EntityID id) {
	this->entities_to_delete.insert(id);
}


void ServerGameState::markAsUpdated(EntityID id) {
	this->updated_entities.insert(id);
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
	SmartVector<Object*> gameObjects = this->objects.getMovableObjects();

	for (int i = 0; i < gameObjects.size(); i++) {
		//	Get iterating game object
		Object* object = gameObjects.get(i);

		//	If the object is a nullptr or isn't movable, skip
		if (object == nullptr || !(object->physics.movable))
			continue;

		glm::vec3 starting_corner_pos = object->physics.shared.corner;

		//	Object is movable - for now, add to updated entities set
		this->updated_entities.insert(object->globalID);

		//	Object is movable - compute total movement step
		glm::vec3 totalMovementStep = 
			object->physics.velocity * object->physics.velocityMultiplier + object->physics.currTickVelocity;
		totalMovementStep.x *= object->physics.nauseous;
		totalMovementStep.z *= object->physics.nauseous;

		if (object->type == ObjectType::DungeonMaster) {
			object->physics.shared.corner += totalMovementStep;

			// Rows * grid cell width = z length and columns * grid cell width = x length
			// check z length
			bool zInBounds = (object->physics.shared.corner.z <= this->grid.getRows() * this->grid.grid_cell_width) && (object->physics.shared.corner.z >= 0);

			bool xInBounds = (object->physics.shared.corner.x <= this->grid.getColumns() * this->grid.grid_cell_width) && (object->physics.shared.corner.x >= 0);

			if (!zInBounds) {
				object->physics.shared.corner.z -= totalMovementStep.z;
			}

			if (!xInBounds) {
				object->physics.shared.corner.x -= totalMovementStep.x;
			}

			continue;
		}

		auto creature = dynamic_cast<Creature*>(object);
		if (creature != nullptr) {
			if (creature->statuses.getStatusLength(Status::Slimed) > 0) {
				totalMovementStep *= 0.5f;
			}
			if (creature->statuses.getStatusLength(Status::Frozen) > 0) {
				totalMovementStep *= 0.0f;
			}
		}

		//	If the object doesn't have a collider, update its movement without
		//	collision detection
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

		//	Object's current position (before current movementStep)
		glm::vec3 currentPosition = object->physics.shared.corner;

		//	Perform collision detection + movement update for each incremental
		//	step (or this loop only iterates once if incremental steps are not
		//	used)
		while (numSteps < NUM_INCREMENTAL_STEPS) {
			numSteps++;

			bool collided = false; 
			bool collidedX = false;
			bool collidedZ = false;

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
				collidedX = this->hasObjectCollided(object,
					glm::vec3(
						currentPosition.x + movementStep.x,
						currentPosition.y,
						currentPosition.z
					));
				
				//	Test for collision when object only moves by movementStep's
				//	z component
				collidedZ = this->hasObjectCollided(object,
					glm::vec3(
						currentPosition.x,
						currentPosition.y,
						currentPosition.z + movementStep.z
					));
			}

			//	Update object's movement

			//	Horizontal movement
			if (collidedX) {
				movementStep.x = 0;
			}
			if (collidedZ) {
				movementStep.z = 0;
			}

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
		if (object->physics.shared.corner.y < 0) {
			//	Clamp object to floor if corner's y position is lower than the floor
			object->physics.shared.corner.y = 0;

			// Play relevant landing sounds
			if (starting_corner_pos.y != 0.0f) {
				if (object->type == ObjectType::Player) {
					this->sound_table.addNewSoundSource(SoundSource(
						ServerSFX::PlayerLand,
						object->physics.shared.corner,
						DEFAULT_VOLUME,
						MEDIUM_DIST,
						MEDIUM_ATTEN
					));
				} else if (object->type == ObjectType::SpikeTrap) {
					this->sound_table.addNewSoundSource(SoundSource(
						ServerSFX::CeilingSpikeImpact,
						object->physics.shared.corner,
						FULL_VOLUME,
						FAR_DIST,
						FAR_ATTEN
					));
				}
			}
		}

		// Play relevant footstep sounds

		// Footstep audio for players
		if (object->type == ObjectType::Player) {
			if (object->distance_moved > 3.0f && object->physics.shared.corner.y == 0.0f) {
				object->distance_moved = 0.0f; // reset so we only play footsteps every so often
				this->sound_table.addNewSoundSource(SoundSource(
					getNextPlayerFootstep(object->globalID),
					object->physics.shared.getCenterPosition(),
					DEFAULT_VOLUME,
					SHORT_DIST,
					SHORT_ATTEN	
				));
			}
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
				|| otherObj->physics.collider == Collider::None
				|| (object->type == ObjectType::Player && otherObj->type == ObjectType::Player)
				|| (object->type == ObjectType::Item && otherObj->type == ObjectType::Player)
				|| (object->type == ObjectType::Player && otherObj->type == ObjectType::Item)) {
				continue;
			}

			//	Detect whether a collision has occurred
			if (detectCollision(object->physics, otherObj->physics))
			{
				//	Add object pair to set of collided objects
				//	Note: object pair is added in increasing order of their
				//	global IDs to avoid inserting the same pair twice in a
				//	different order (e.g. {object, otherObj} and 
				//	{otherObj, object} shouldn't be treated as two separate
				//	object collision pairs)
				if (object->globalID < otherObj->globalID) {
					this->collidedObjects.insert({ object, otherObj });
				}
				else {
					this->collidedObjects.insert({ otherObj, object });
				}

				//	Exception - if the other object is a floor spike trap,
				//	perform collision handling but do not return true as the
				//	trap doesn't affect the movement of the object it hits
				if (otherObj->type == ObjectType::FloorSpike || 
					otherObj->type == ObjectType::Potion || 
					otherObj->type == ObjectType::Spell ||
					otherObj->type == ObjectType::Weapon ||
					otherObj->type == ObjectType::Orb ||
					otherObj->type == ObjectType::WeaponCollider ||
					otherObj->type == ObjectType::Slime ||
					otherObj->type == ObjectType::Torchlight) {
					continue;
				}

				return true;
			}
		}
	}
	
	return false;
}

void ServerGameState::spawnEnemies() {
	this->spawner->spawn(*this);
}

void ServerGameState::updateItems() {
	auto items = this->objects.getItems();
	for (int i = 0; i < items.size(); i++) {
		auto item = items.get(i);
		if (item == nullptr) { continue; }

		if (item->physics.movable && item->physics.shared.corner.y == 0) {
			item->physics.velocity.x = 0;
			item->physics.velocity.z = 0;
		}

		if (item->type == ObjectType::Potion) {
			Potion* pot = dynamic_cast<Potion*>(item);
			if (pot->iteminfo.used) {
				if (pot->timeOut()) {
					pot->revertEffect(*this);
					this->updated_entities.insert(pot->globalID);
				}
			}
		}

		if (item->type == ObjectType::Weapon) {
			Weapon* weapon = dynamic_cast<Weapon*>(item);
			weapon->reset(*this);
		}
	}
}

void ServerGameState::updateEnemies() {
	auto enemies = this->objects.getEnemies();

	for (int e = 0; e < enemies.size(); e++){
		auto enemy = enemies.get(e);
		if (enemy == nullptr) continue;

		if (enemy->doBehavior(*this)) {
			this->updated_entities.insert(enemy->globalID);
		}
	}
}

void ServerGameState::doProjectileTicks() {
	auto projectiles = this->objects.getProjectiles();
	for (int p = 0; p < projectiles.size(); p++) {
		auto projectile = projectiles.get(p);
		if (projectile == nullptr) continue;

		if (projectile->doTick(*this)) {
			this->updated_entities.insert(projectile->globalID);
		}
	}
}

void ServerGameState::updateAttacks() {
	auto weaponColliders = this->objects.getWeaponColliders();
	for (int i = 0; i < weaponColliders.size(); i++) {
		auto weaponCollider = weaponColliders.get(i);
		if (weaponCollider == nullptr) { continue; }
		weaponCollider->updateMovement(*this);
		if(weaponCollider->readyTime(*this)){
			if (weaponCollider->timeOut(*this)) {
				this->markForDeletion(weaponCollider->globalID);
			}
			else {
				this->updated_entities.insert(weaponCollider->globalID);
				continue;
			}
		}
		this->updated_entities.insert(weaponCollider->globalID);
	}
}

void ServerGameState::doTorchlightTicks() {
	auto torchlights = this->objects.getTorchlights();
	for (int t = 0; t < torchlights.size(); t++) {
		auto torchlight = torchlights.get(t);
		if (torchlight == nullptr) continue;

		torchlight->doTick(*this);
	}
}

void ServerGameState::updateTraps() {
	// get current time when calling this function
	auto current_time = std::chrono::system_clock::now();
	DungeonMaster* dm = this->objects.getDM();

	// update DM trap cooldown
	if (this->objects.getDM() != nullptr) {
		auto& coolDownMap = dm->sharedTrapInventory.trapsInCooldown;

		//std::cout << coolDownMap << " cooldown map size" << std::endl;

		for (auto it = dm->sharedTrapInventory.trapsInCooldown.cbegin(); it != dm->sharedTrapInventory.trapsInCooldown.cend();) {
			if (std::chrono::round<std::chrono::seconds>(current_time - std::chrono::system_clock::from_time_t(it->second)) >= std::chrono::seconds(TRAP_COOL_DOWN)) {
				it = dm->sharedTrapInventory.trapsInCooldown.erase(it);
			}
			else {
				it++;
			}
		}
	}

	// This object moved, so we should check to see if a trap should trigger because of it
	auto traps = this->objects.getTraps();
	for (int i = 0; i < traps.size(); i++) {
		auto trap = traps.get(i);
		if (trap == nullptr) { continue; } // unsure if i need this?
		if (trap->getIsDMTrap()) {
			
			if (current_time >= trap->getExpiration()) {
				
				int trapsPlaced = dm->getPlacedTraps();

				this->markForDeletion(trap->globalID);
				dm->setPlacedTraps(trapsPlaced - 1);
				continue;
			}
		}

		// check for activations
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
			//	Player died - increment number of player deaths
			this->numPlayerDeaths++;

			if (numPlayerDeaths == PLAYER_DEATHS_TO_RELAY_RACE) {
				this->transitionToRelayRace();
			}

			//handle dropping items (sets collider to none so it doesn't pick up items when dead)
			player->physics.collider = Collider::None;
			for (int i = 0; i < player->sharedInventory.inventory_size; i++) {
				if (player->inventory[i] != -1) {
            		auto item = dynamic_cast<Item*>(this->objects.getItem(player->inventory[i]));
					item->dropItem(player, *this, i, 0.0f);
					this->updated_entities.insert(item->globalID);
				}
				// hardcode "random" drops
				if (i == 1){ player->physics.shared.facing *= -1.0f; }
				if (i == 2){ player->physics.shared.facing = glm::vec3(0.5f, 0, 0.7f); }
				if (i == 3){ player->physics.shared.facing = glm::vec3(-0.3f, 0, 0.1f); }
			}

			// remove pot effects when killed
			for (auto it = player->sharedInventory.usedItems.begin(); it != player->sharedInventory.usedItems.end(); ) {
				auto item = dynamic_cast<Item*>(this->objects.getItem(it->first));
				if (item->type == ObjectType::Potion) {
					Potion* pot = dynamic_cast<Potion*>(item);
					it = pot->revertEffect(*this);
					this->updated_entities.insert(pot->globalID);
				} else {
					it++;
				}
			}

			this->updated_entities.insert(player->globalID);
			player->physics.velocity = glm::vec3(0.0f);
			player->info.is_alive = false;
			player->info.respawn_time = getMsSinceEpoch() + 5000; // currently hardcode to wait 5s
		}
	}

	auto enemies = this->objects.getEnemies();
	for (int e = 0; e < enemies.size(); e++) {
		auto enemy = enemies.get(e);
		if (enemy == nullptr) continue;

		if (enemy->stats.health.current() <= 0) {
			this->updated_entities.insert(enemy->globalID);
			if (enemy->doDeath(*this)) {
				this->entities_to_delete.insert(enemy->globalID);
			}
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
				player->physics.collider = Collider::Box;
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

void ServerGameState::tickStatuses() {
	auto players = this->objects.getPlayers();
	for (auto p = 0; p < players.size(); p++) {
		auto player = players.get(p);
		if (player == nullptr) continue;

		player->statuses.tickStatus();
	}
	auto enemies = this->objects.getEnemies();
	for (auto e = 0; e < players.size(); e++) {
		auto enemy = enemies.get(e);
		if (enemy == nullptr) continue;

		enemy->statuses.tickStatus();
	}
}

void ServerGameState::handleTickVelocity() {
	auto players = this->objects.getPlayers();
	for (auto p = 0; p < players.size(); p++) {
		auto player = players.get(p);
		if (player == nullptr) continue;

		// is this actually the best i can do...?
		if (player->physics.currTickVelocity != glm::vec3(0.0f)) {
			if (player->physics.currTickVelocity.x > 0) {
				player->physics.currTickVelocity.x -= 0.05f;
			}
			else if (player->physics.currTickVelocity.x < 0) {
				player->physics.currTickVelocity.x += 0.05f;
			}

			if (player->physics.currTickVelocity.y > 0) {
				player->physics.currTickVelocity.y -= 0.05f;
			}
			else if (player->physics.currTickVelocity.y < 0) {
				player->physics.currTickVelocity.y += 0.05f;
			}

			if (player->physics.currTickVelocity.z > 0) {
				player->physics.currTickVelocity.z -= 0.05f;
			}
			else if (player->physics.currTickVelocity.z < 0) {
				player->physics.currTickVelocity.z += 0.05f;
			}

			if (abs(player->physics.currTickVelocity.x) <= 0.05f) {
				player->physics.currTickVelocity.x = 0.0f;
			}
			if (abs(player->physics.currTickVelocity.y) <= 0.05f) {
				player->physics.currTickVelocity.y = 0.0f;
			}
			if (abs(player->physics.currTickVelocity.z) <= 0.05f) {
				player->physics.currTickVelocity.z = 0.0f;
			}
		}
	}

	auto enemies = this->objects.getEnemies();
	for (auto e = 0; e < enemies.size(); e++) {
		auto enemy = enemies.get(e);
		if (enemy == nullptr) continue;

		if (enemy->physics.currTickVelocity != glm::vec3(0.0f)) {
			if (enemy->physics.currTickVelocity.x > 0) {
				enemy->physics.currTickVelocity.x -= 0.05f;
			}
			else if (enemy->physics.currTickVelocity.x < 0) {
				enemy->physics.currTickVelocity.x += 0.05f;
			}

			if (enemy->physics.currTickVelocity.y > 0) {
				enemy->physics.currTickVelocity.y -= 0.05f;
			}
			else if (enemy->physics.currTickVelocity.y < 0) {
				enemy->physics.currTickVelocity.y += 0.05f;
			}

			if (enemy->physics.currTickVelocity.z > 0) {
				enemy->physics.currTickVelocity.z -= 0.05f;
			}
			else if (enemy->physics.currTickVelocity.z < 0) {
				enemy->physics.currTickVelocity.z += 0.05f;
			}

			if (abs(enemy->physics.currTickVelocity.x) <= 0.05f) {
				enemy->physics.currTickVelocity.x = 0.0f;
			}
			if (abs(enemy->physics.currTickVelocity.y) <= 0.05f) {
				enemy->physics.currTickVelocity.y = 0.0f;
			}
			if (abs(enemy->physics.currTickVelocity.z) <= 0.05f) {
				enemy->physics.currTickVelocity.z = 0.0f;
			}
		}
	}
}


unsigned int ServerGameState::getTimestep() const {
	return this->timestep;
}

GamePhase ServerGameState::getPhase() const {
	return this->phase;
}

void ServerGameState::setPhase(GamePhase phase) {
	this->phase = phase;
}

MatchPhase ServerGameState::getMatchPhase() const {
	return this->matchPhase;
}

void ServerGameState::transitionToRelayRace() {
	//	Return immediately if the match phase is already Relay Race
	if (this->matchPhase == MatchPhase::RelayRace)
		return;

	this->matchPhase = MatchPhase::RelayRace;

	//	Open all exits!
	for (int i = 0; i < this->objects.getExits().size(); i++) {
		Exit* exit = this->objects.getExits().get(i);

		if (exit == nullptr)
			continue;

		exit->shared.open = true;
	}
}

void ServerGameState::setPlayerVictory(bool playerVictory) {
	this->playerVictory = playerVictory;
}

void ServerGameState::addPlayerToLobby(LobbyPlayer player) {
	//this->lobby.players[id] = name;

	//	Only add the player if a player with the given EntityID doesn't exist
	for (int i = 0; i < this->lobby.max_players; i++) {
		if (this->lobby.players[i].has_value()
			&& this->lobby.players[i].get().id == player.id)
		{
			//	A player with this EntityID already exists in this
			//	ServerGameState's Lobby - return without adding player
			//	again to this server's Lobby
			return;
		}
	}

	bool freeIndex = false;

	for (int i = 0; i < this->lobby.max_players; i++) {
		if (!this->lobby.players[i].has_value()) {
			//	Found a free index! Adding player here
			freeIndex = true;
			this->lobby.players[i] = player;
			std::cout << "Added new player in index " << std::to_string(i) << std::endl;
			std::cout << "Player's eid: " << player.id << std::endl;
			break;
		}
	}

	//	Crash server if no free index was found
	assert(freeIndex);
}

void ServerGameState::updateLobbyPlayer(EntityID id, LobbyPlayer player) {
	//	Iterate through the players vector and update the player with the given
	//	EntityID
	for (int i = 0; i < this->lobby.max_players; i++) {
		if (!this->lobby.players[i].has_value())
			continue;

		if (this->lobby.players[i].get().id == id) {
			//	Update player
			this->lobby.players[i] = player;
		}
	}
}

void ServerGameState::removePlayerFromLobby(EntityID id) {
	//	Iterate through the players vector and remove the player with the given
	//	EntityID
	for (int i = 0; i < this->lobby.max_players; i++) {
		if (!this->lobby.players[i].has_value())
			continue;

		if (this->lobby.players[i].get().id == id) {
			//	Remove player
			this->lobby.players[i] = boost::none;
		}
	}

	//	Note: this method doesn't check that the removal was successful. It's
	//	possible that an EntityID was passed in that no player in the lobby has
	//	and so the removal had no effect
}

const Lobby& ServerGameState::getLobby() const {
	return this->lobby;
}

Trap* ServerGameState::placeTrapInCell(GridCell* cell, CellType type) {
	switch (type) {
	case CellType::FireballTrapLeft:
	case CellType::FireballTrapRight:
	case CellType::FireballTrapUp:
	case CellType::FireballTrapDown: {
		if (cell->type != CellType::Empty)
			return nullptr;
        return spawnFireballTrap(cell);
	}
	case CellType::SpikeTrap: {
		if (cell->type != CellType::Empty)
			return nullptr;

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

		SpikeTrap* spikeTrap = new SpikeTrap(corner, dimensions);
		this->objects.createObject(spikeTrap);
		return spikeTrap;
	}
	/*
	* TODO: ADD BACK FAKEWALL
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
		}
		break;
	}*/
	case CellType::FloorSpikeFull:
	case CellType::FloorSpikeHorizontal:
	case CellType::FloorSpikeVertical: {
		if (cell->type != CellType::Empty) {
			return nullptr;
		}

		glm::vec3 corner(
			cell->x * Grid::grid_cell_width,
			0.0f,
			cell->y * Grid::grid_cell_width
		);

		FloorSpike::Orientation orientation;
		if (type == CellType::FloorSpikeFull) {
			orientation = FloorSpike::Orientation::Full;
		}
		else if (type == CellType::FloorSpikeHorizontal) {
			orientation = FloorSpike::Orientation::Horizontal;
			corner.z += Grid::grid_cell_width * 0.25f;
		}
		else {
			orientation = FloorSpike::Orientation::Vertical;
			corner.x += Grid::grid_cell_width * 0.25f;
		}

		FloorSpike* floorSpike = new FloorSpike(corner, orientation, Grid::grid_cell_width);
		this->objects.createObject(floorSpike);
		return floorSpike;
	}
	/*
	TODO: ADD BACK ARROWS!

	case CellType::ArrowTrapDown:
	case CellType::ArrowTrapLeft:
	case CellType::ArrowTrapRight:
	case CellType::ArrowTrapUp: {
		ArrowTrap::Direction dir;
		if (cell->type == CellType::ArrowTrapDown) {
			dir = ArrowTrap::Direction::DOWN;
		}
		else if (cell->type == CellType::ArrowTrapUp) {

		}
		else if (cell->type == CellType::ArrowTrapLeft) {
			dir = ArrowTrap::Direction::LEFT;
		}
		else {
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
	}*/

	case CellType::TeleporterTrap: {
		if (cell->type != CellType::Empty)
			return nullptr;

		glm::vec3 corner(
			cell->x * Grid::grid_cell_width,
			0.0f,
			cell->y * Grid::grid_cell_width
		);

		TeleporterTrap* teleporterTrap = new TeleporterTrap(corner);
		this->objects.createObject(teleporterTrap);
		return teleporterTrap;
	}
	default: {
		std::cerr << "WARNING: tried to place an unknown trap of type " << static_cast<int>(type) << "\n";
		return nullptr;
	}
	}
}

/*	Maze initialization	*/

void ServerGameState::loadMaze(const Grid& grid) {
	this->grid = grid;

	//	Verify that there's at least one spawn point
	size_t num_spawn_points = this->grid.getSpawnPoints().size();
	assert(num_spawn_points > 0);

	// mark internal walls in this set, so we can mark them appropriately later on
	std::unordered_set<glm::ivec2> internal_walls;
	for (int c = 0; c < this->grid.getColumns(); c++) {
		for (int r = 0; r < this->grid.getRows(); r++) {
            int num_neighbor_walls = 0;            

            for (int offset_row = -1; offset_row <= 1; offset_row++) {
                for (int offset_col = -1; offset_col <= 1; offset_col++) {
                    if (offset_row == 0 && offset_col == 0) continue;

                    int n_row = r + offset_row;
                    int n_col = c + offset_col;

                    bool is_wall;
                    if (n_row < 0 || n_row >= this->grid.getRows() || n_col < 0 || n_col >= this->grid.getColumns()) {
                        is_wall = true; // outside, but for the purposes of the algorithm still consider wall
                    } else {
                        is_wall = isWallLikeCell(this->grid.getCell(n_col, n_row)->type);
                    }

                    if (is_wall) {
                        num_neighbor_walls++;
                    }
                }
            }

			if (num_neighbor_walls == 8) {
				internal_walls.insert(glm::ivec2(c, r));
			}
		}
	}

	//	Step 5:	Add floor and ceiling SolidSurfaces.

	// Create Ceiling
	this->objects.createObject(new SolidSurface(false, Collider::Box, SurfaceType::Ceiling, 
		glm::vec3(0.0f, MAZE_CEILING_HEIGHT, 0.0f),
		glm::vec3(this->grid.getColumns() * Grid::grid_cell_width, 0.1,
			this->grid.getRows() * Grid::grid_cell_width)
	));

	SolidSurface* floor = new SolidSurface(false, Collider::None, SurfaceType::Floor,
		glm::vec3(0.0f, -0.1f, 0.0f),
		glm::vec3(this->grid.getColumns() * Grid::grid_cell_width, 0.1,
			this->grid.getRows() * Grid::grid_cell_width)
	);
	this->objects.createObject(floor);

	// this is for floor highlighting
	std::vector<std::vector<bool>> freeSpots(grid.getRows(), std::vector<bool>(grid.getColumns(), false));

	//	Step 6:	For each GridCell, add an object (if not empty) at the 
	//	GridCell's position.
	for (int row = 0; row < this->grid.getRows(); row++) {
		for (int col = 0; col < this->grid.getColumns(); col++) {

			GridCell* cell = this->grid.getCell(col, row);

			if (cell->type == CellType::RandomPotion) {
				int r = randomInt(1, 100);
				if (r < 25) {
					cell->type = CellType::HealthPotion;
				} else if (r < 50) {
					cell->type = CellType::InvisibilityPotion;
				} else if (r < 75) {
					cell->type = CellType::InvincibilityPotion;
				} else {
					cell->type = CellType::NauseaPotion;
				}
			} else if (cell->type == CellType::RandomSpell) {
				int r = randomInt(1, 3);
				if (r == 1) {
					cell->type = CellType::FireSpell;
				} else if (r == 2) {
					cell->type = CellType::HealSpell;
				} else {
					cell->type = CellType::TeleportSpell;
				}
			} else if (cell->type == CellType::RandomWeapon) {
				std::cout << "weapon row: " << cell->x << " weapon col: " << cell->y << std::endl;
				int r = randomInt(1, 3);
				if (r == 1) {
					cell->type = CellType::Dagger;
				}
				else if (r == 2) {
					cell->type = CellType::Sword;
				}
				else {
					cell->type = CellType::Hammer;
				}
			}

			switch (cell->type) {
				case CellType::Orb: {
					glm::vec3 dimensions(1.0f);

					glm::vec3 corner(
						cell->x * Grid::grid_cell_width + 1,
						0,
						cell->y * Grid::grid_cell_width + 1);

					this->objects.createObject(new Orb(corner, dimensions));
					break;
				}
				case CellType::FireballTrapLeft:
                case CellType::FireballTrapRight:
                case CellType::FireballTrapUp:
                case CellType::FireballTrapDown:
                    spawnFireballTrap(cell);
					break;
				case CellType::Dagger: {
					glm::vec3 dimensions(1.0f);

					glm::vec3 corner(
						cell->x * Grid::grid_cell_width + 1,
						0,
						cell->y * Grid::grid_cell_width + 1);

					this->objects.createObject(new Weapon(corner, dimensions, WeaponType::Dagger));
					break;
				}
				case CellType::Sword: {
					glm::vec3 dimensions(1.0f);

					glm::vec3 corner(
						cell->x * Grid::grid_cell_width + 1,
						0,
						cell->y * Grid::grid_cell_width + 1);

					this->objects.createObject(new Weapon(corner, dimensions, WeaponType::Sword));
					break;
				}
				case CellType::Hammer: {
					glm::vec3 dimensions(1.0f);

					glm::vec3 corner(
						cell->x * Grid::grid_cell_width + 1,
						0,
						cell->y * Grid::grid_cell_width + 1);

					this->objects.createObject(new Weapon(corner, dimensions, WeaponType::Hammer));
					break;
				}
				case CellType::TeleportSpell: {
					glm::vec3 dimensions(1.0f);

					glm::vec3 corner(
						cell->x * Grid::grid_cell_width + 1,
						0,
						cell->y * Grid::grid_cell_width + 1);

					this->objects.createObject(new Spell(corner, dimensions, SpellType::Teleport));
					break;
				}
				case CellType::FireSpell: {
					glm::vec3 dimensions(1.0f);

					glm::vec3 corner(
						cell->x * Grid::grid_cell_width + 1,
						0,
						cell->y * Grid::grid_cell_width + 1);

					this->objects.createObject(new Spell(corner, dimensions, SpellType::Fireball));
					break;
				}
				case CellType::HealSpell: {
					glm::vec3 dimensions(1.0f);

					glm::vec3 corner(
						cell->x * Grid::grid_cell_width + 1,
						0,
						cell->y * Grid::grid_cell_width + 1);

					this->objects.createObject(new Spell(corner, dimensions, SpellType::HealOrb));
					break;
				}
				case CellType::HealthPotion: {
					std::cout << "health potion row: " << cell->x << " health potion col: " << cell->y << std::endl;
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
				case CellType::InvincibilityPotion: {
					glm::vec3 dimensions(1.0f);

					glm::vec3 corner(cell->x * Grid::grid_cell_width + 1,
						0,
						cell->y * Grid::grid_cell_width + 1);

					this->objects.createObject(new Potion(corner, dimensions, PotionType::Invincibility));
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
					this->objects.createObject(new Slime(
						this->grid.gridCellCenterPosition(cell),
						glm::vec3(0.0f),
						3
					));
					break;
				}
				case CellType::Wall:
				case CellType::Pillar:
				case CellType::FakeWall: {
                    this->spawnWall(cell, col, row, internal_walls.contains(glm::ivec2(col, row)));
					break;
				}
                case CellType::TorchUp:
                case CellType::TorchDown:
                case CellType::TorchRight:
                case CellType::TorchLeft: {
                    this->spawnTorch(cell);
                    this->spawnWall(cell, col, row, internal_walls.contains(glm::ivec2(col, row)));
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
					Direction dir;
					if (cell->type == CellType::ArrowTrapDown) {
						dir = Direction::DOWN;
					} else if (cell->type == CellType::ArrowTrapUp) {
						dir = Direction::UP;
					} else if (cell->type == CellType::ArrowTrapLeft) {
						dir = Direction::LEFT;
					} else {
						dir = Direction::RIGHT;
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

				case CellType::TeleporterTrap: {
					glm::vec3 corner(
						cell->x * Grid::grid_cell_width,
						0.0f,
						cell->y * Grid::grid_cell_width
					);

					this->objects.createObject(new TeleporterTrap(corner));
					break;
				}
				case CellType::Exit: {
					glm::vec3 corner(
						cell->x* Grid::grid_cell_width,
						0.0f,
						cell->y* Grid::grid_cell_width
					);

					glm::vec3 dimensions(
						Grid::grid_cell_width,
						MAZE_CEILING_HEIGHT,
						Grid::grid_cell_width
					);

					this->objects.createObject(new Exit(false, corner, dimensions));
					break;
				}
				default: {
					// available spot for placement
					solidSurfaceInGridCells.insert({row, col});
				}
			}
		}
	}
}

void ServerGameState::spawnWall(GridCell* cell, int col, int row, bool is_internal) {
    glm::vec3 dimensions(
        this->grid.grid_cell_width,
        MAZE_CEILING_HEIGHT,
        this->grid.grid_cell_width
    );
    glm::vec3 corner(
        cell->x * this->grid.grid_cell_width,
        0.0f, 
        cell->y * this->grid.grid_cell_width
    );

    if (cell->type == CellType::FakeWall) {
        this->objects.createObject(new FakeWall(corner, dimensions));
    } else if (cell->type == CellType::Wall ||
        cell->type == CellType::TorchUp ||
        cell->type == CellType::TorchDown ||
        cell->type == CellType::TorchLeft ||
        cell->type == CellType::TorchRight ||
		cell->type == CellType::Pillar) {

		SurfaceType surface_type = (cell->type == CellType::Pillar) ? SurfaceType::Pillar : SurfaceType::Wall;
		SolidSurface* wall = new SolidSurface(false, Collider::Box, surface_type, corner, dimensions);
		wall->shared.is_internal = is_internal;
        this->objects.createObject(wall);
		if (cell->type == CellType::Wall || cell->type == CellType::Pillar) {
			// don't let the DM select walls with torches
			solidSurfaceInGridCells.insert({col, row});
		}	
    }
}

void ServerGameState::spawnTorch(GridCell *cell) {
    glm::vec3 dimensions = Object::models.at(ModelType::Torchlight);
    glm::vec3 corner(
        cell->x * this->grid.grid_cell_width,
        MAZE_CEILING_HEIGHT / 2.0f,
        cell->y * this->grid.grid_cell_width
    );

    switch (cell->type) {
        case CellType::TorchDown: {
            corner.x += (this->grid.grid_cell_width / 2.0f) - (dimensions.x / 2.0f); 
            corner.z += this->grid.grid_cell_width;
            break;
        }
        case CellType::TorchUp: {
            corner.x += (this->grid.grid_cell_width / 2.0f) - (dimensions.x / 2.0f); 
            corner.z -= dimensions.z;
            break;
        }
        case CellType::TorchLeft: {
            corner.x -= dimensions.x; 
            corner.z += (this->grid.grid_cell_width / 2.0f) - (dimensions.z / 2.0f); 
            break;
        }
        case CellType::TorchRight: {
            corner.x += this->grid.grid_cell_width;
            corner.z += (this->grid.grid_cell_width / 2.0f) - (dimensions.z / 2.0f);
            break;
        }
        default: {
            std::cout << "Invalid Torch cell type when spawning torch\n";
        }
    }

	// Add an entry in the sound table for this
	this->sound_table.addStaticSoundSource(SoundSource(
		ServerSFX::TorchLoop,
		corner,
		MIDDLE_VOLUME,
		SHORT_DIST,
		SHORT_ATTEN,
		true
	));

    this->objects.createObject(new Torchlight(corner));
}

Trap* ServerGameState::spawnFireballTrap(GridCell *cell) {
    glm::vec3 dimensions = Object::models.at(ModelType::SunGod);
    glm::vec3 corner(
        (cell->x * Grid::grid_cell_width),
        0.0f,
        (cell->y * Grid::grid_cell_width)
    );
    Direction dir;
    switch (cell->type) {
        case CellType::FireballTrapLeft:
            dir = Direction::LEFT;
            // corner.z -= (dimensions.z / 2.0f);
            break;
        case CellType::FireballTrapRight:
            dir = Direction::RIGHT;
            // corner.z -= (dimensions.z / 2.0f);
            break;
        case CellType::FireballTrapUp:
            dir = Direction::UP;
            corner.x += (dimensions.x / 2.0f);
            break;
        case CellType::FireballTrapDown:
            corner.x += (dimensions.x / 2.0f);
            dir = Direction::DOWN;
            break;
        default:
            dir = Direction::LEFT;
            break;
    }
    FireballTrap* fireBallTrap = new FireballTrap(corner, dir);
    this->objects.createObject(fireBallTrap);
    return fireBallTrap;
}

Grid& ServerGameState::getGrid() {
	return this->grid;
}

/*	Debugger Methods	*/

std::string ServerGameState::to_string() {
	std::string representation = "{";
	representation += "\n\ttimestep:\t\t" + std::to_string(this->timestep);
	representation += "\n\ttimestep len:\t\t" + std::to_string(TIMESTEP_LEN.count());
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