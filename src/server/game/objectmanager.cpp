#include "server/game/objectmanager.hpp"
#include "server/game/enemy.hpp"
#include "server/game/spiketrap.hpp"
#include "server/game/fireballtrap.hpp"
#include "server/game/projectile.hpp"
#include "server/game/potion.hpp"

#include <memory>

/*	Constructors and Destructors	*/

ObjectManager::ObjectManager() {
	////	Initialize global SmartVector
	//this->objects = SmartVector<Object*>();

	////	Initialize type-specific SmartVectors
	//this->base_objects = SmartVector<Object*>();
	//this->items = SmartVector<Item*>();
}

ObjectManager::~ObjectManager() {
	//	TODO: Delete all allocated objects
}

/*	Object CRUD methods	*/

SpecificID ObjectManager::createObject(Object* object) {
	//	Create a new object with the given type
	EntityID globalID = this->objects.push(object);
	object->globalID = globalID;

	object->gridCellPositions = this->objectGridCells(object);
	for (auto pos : object->gridCellPositions) {
		if (!this->cellToObjects.contains(pos)) {
			this->cellToObjects.insert({pos, std::vector<Object*>()});
		}
	}

	switch (object->type) {
		case ObjectType::Projectile:
			object->typeID = this->projectiles.push(dynamic_cast<Projectile*>(object));
			break;
		case ObjectType::FireballTrap:
		case ObjectType::FakeWall:
		case ObjectType::SpikeTrap:
		case ObjectType::FloorSpike:
		case ObjectType::ArrowTrap:
			object->typeID = this->traps.push(dynamic_cast<Trap*>(object));
			break;
		case ObjectType::Potion:
			object->typeID = this->items.push(dynamic_cast<Potion*>(object));
			break;
		case ObjectType::SolidSurface:
			object->typeID = this->solid_surfaces.push(dynamic_cast<SolidSurface*>(object));
			break;
		case ObjectType::Player:
			object->typeID = this->players.push(dynamic_cast<Player*>(object));
			break;
        case ObjectType::Enemy:
			object->typeID = this->enemies.push(dynamic_cast<Enemy*>(object));
			break;
        default:
			std::cerr << "FATAL: invalid object type being created: " << static_cast<int>(object->type) << 
				"\nDid you remember to add a new switch statement to ObjectManager::createObject?\n";
			std::exit(1);
	}

	//	Move object to its given position
	moveObject(object, object->physics.shared.corner);

	return object->typeID;
}

bool ObjectManager::removeObject(EntityID globalID) {
	//	Check that the given object exists
	Object* object = this->objects.get(globalID);

	if (object == nullptr) {
		//	Object with the given index doesn't exist
		return false;
	}

	//	Remove object from the global objects SmartVector and from the
	//	type-specific Object vector it's in
	this->objects.remove(globalID);

	switch (object->type) {
	case ObjectType::Object:
		//	Remove object pointer from the base_objects type-specific 
		//	SmartVector
		this->base_objects.remove(object->typeID);
		break;
	case ObjectType::FireballTrap:
	case ObjectType::SpikeTrap:
		this->traps.remove(object->typeID);
		break;
	case ObjectType::Item:
		this->items.remove(object->typeID);
		break;
	case ObjectType::Player:
		this->players.remove(object->typeID);
		break;
	case ObjectType::Projectile:
		this->projectiles.remove(object->typeID);
		break;
	case ObjectType::Enemy:
		this->enemies.remove(object->typeID);
		break;
	case ObjectType::Potion:
		this->items.remove(object->typeID);
		break;
	}

	//	Remove object from cellToObjects hashmap
	for (glm::vec2 cellPosition : object->gridCellPositions) {
		std::vector<Object*>& objectsInCell =
			this->cellToObjects[cellPosition];

		//	Remove object from Object * vector of objects in this cell
		for (int i = 0; i < objectsInCell.size(); i++) {
			if (objectsInCell.at(i)->globalID == object->globalID) {
				objectsInCell.erase(objectsInCell.begin() + i);
				break;
			}
		}
	}

	//	Delete object
	delete object;

	return true;
}

bool ObjectManager::removeObject(Object** object_dbl_ptr) {
	//	Check that the given object double pointer isn't nullptr or that the
	//	object pointer it points to isn't nullptr
	if (object_dbl_ptr == nullptr || *object_dbl_ptr == nullptr)
		return false;

	//	Object pointer points to an Object - attempt to remove it
	Object* object = *object_dbl_ptr;

	//	Get EntityID of this object
	EntityID globalID = object->globalID;

	if (this->removeObject(globalID)) {
		//	Set object pointer to nullptr (avoids dangling pointers)
		*object_dbl_ptr = nullptr;
		return true;
	}

	//	Failed to remove object
	return false;
}

Object* ObjectManager::getObject(EntityID globalID) {
	return this->objects.get(globalID);
}

SmartVector<Object*> ObjectManager::getObjects() {
	return this->objects;
}

/*	SpecificID object getters by type	*/
Object* ObjectManager::getBaseObject(SpecificID base_objectID) {
	return this->base_objects.get(base_objectID);
}

Item* ObjectManager::getItem(SpecificID itemID) {
	return this->items.get(itemID);
}

SolidSurface* ObjectManager::getSolidSurface(SpecificID surfaceID) {
	return this->solid_surfaces.get(surfaceID);
}

Player* ObjectManager::getPlayer(SpecificID playerID) {
	return this->players.get(playerID);
}

Enemy* ObjectManager::getEnemy(SpecificID enemyID) {
	return this->enemies.get(enemyID);
}

Trap* ObjectManager::getTrap(SpecificID trapID) {
	return this->traps.get(trapID);
}

SmartVector<Item*> ObjectManager::getItems() {
	return this->items;
}

SmartVector<SolidSurface*> ObjectManager::getSolidSurfaces() {
	return this->solid_surfaces;
}

SmartVector<Player*> ObjectManager::getPlayers() {
	return this->players;
}

SmartVector<Enemy*> ObjectManager::getEnemies() {
	return this->enemies;
}

SmartVector<Trap*> ObjectManager::getTraps() {
	return this->traps;
}

SmartVector<Projectile*> ObjectManager::getProjectiles() {
	return this->projectiles;
}

/*	Object Movement	*/
bool ObjectManager::moveObject(Object* object, glm::vec3 newCornerPosition) {
	if (object == nullptr) {
		return false;
	}


	//	Remove the object from the cellToObjects hashmap
	for (auto cellPosition : object->gridCellPositions) {
		std::vector<Object*>& objectsInCell = this->cellToObjects.at(cellPosition);

		//	Remove object from Object * vector of objects in this cell
		for (int i = 0; i < objectsInCell.size(); i++) {
			if (objectsInCell.at(i)->globalID == object->globalID) {
				objectsInCell.erase(objectsInCell.begin() + i);
				break;
			}
		}
	}


	//	Update object's corner position
	object->physics.shared.corner = newCornerPosition;

	// std::cout << "before: \n";
	// std::cout << object->gridCellPositions.size() << "\n";
	// std::cout << "now\n";

	//	Get the object's new occupied GridCell position vector
	object->gridCellPositions = objectGridCells(object);

	// std::cout << "before: \n";
	// std::cout << object->gridCellPositions.size() << "\n";
	// std::cout << "now\n";

	// for (int i = 0; i < object->gridCellPositions.size(); i++) {
	// 	std::cout << "object->gridCellPositions[i]=" << object->gridCellPositions[i].x << ", " << object->gridCellPositions[i].y << "\n";
	// }

	for (int i = 0; i < object->gridCellPositions.size(); i++) {
		std::cout << "object->gridCellPositions["<< i << "]=" << object->gridCellPositions[i].x << ", " << object->gridCellPositions[i].y << "\n";
		std::cout << object->gridCellPositions.size() << "\n";
		this->cellToObjects.at(object->gridCellPositions[i]).push_back(object);
	}

	//	Add object to cellToObjects hashmap
	// for (auto cellPosition : object->gridCellPositions) {
	// 	// std::cout << object->gridCellPositions.size() << "|";
	// 	this->cellToObjects.at(cellPosition).push_back(object);
	// }
}

std::vector<glm::ivec2> ObjectManager::objectGridCells(Object* object) {
	return Grid::getCellsFromPositionRange(object->physics.shared.corner,
		object->physics.shared.corner + object->physics.shared.dimensions);
}

/*	SharedGameState generation	*/

std::vector<boost::optional<SharedObject>> ObjectManager::toShared() {
	std::vector<boost::optional<SharedObject>> shared;

	for (int i = 0; i < this->objects.size(); i++) {
		Object* object = this->objects.get(i);

		if (object == nullptr) {
			shared.push_back({});
		} else {
			shared.push_back(object->toShared());
		}
	}

	return shared;
}