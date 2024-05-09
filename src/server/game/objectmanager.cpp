#include "server/game/objectmanager.hpp"
#include "server/game/enemy.hpp"
#include "server/game/spiketrap.hpp"

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

SpecificID ObjectManager::createObject(ObjectType type) {
	//	Create a new object with the given type
	EntityID globalID;
	SpecificID typeID;

	switch (type) {
		case ObjectType::SpikeTrap: {
			SpikeTrap* trap = new SpikeTrap();

			typeID = (SpecificID) this->traps.push(trap);
			globalID = (EntityID) this->objects.push(trap);

			trap->typeID = typeID;
			trap->globalID = globalID;
			break;
		}
		case ObjectType::Item: {
			//	Create a new object of type Item
			Item* item = new Item();

			//	Push to type-specific items vector
			typeID = (SpecificID)this->items.push(item);

			//	Push to global objects vector
			globalID = (EntityID)this->objects.push(item);

			//	Set items' type and global IDs
			item->typeID = typeID;
			item->globalID = globalID;
			break;
		}
		case ObjectType::SolidSurface: {
			//	Create a new object of type SolidSurface
			SolidSurface* solidSurface = new SolidSurface();

			//	Push to type-specific solid_surfaces vector
			typeID = (SpecificID)this->solid_surfaces.push(solidSurface);

			//	Push to global objects vector
			globalID = (EntityID)this->objects.push(solidSurface);

			//	Set solidSurface's type and global IDs
			solidSurface->typeID = typeID;
			solidSurface->globalID = globalID;
			break;
		}
		case ObjectType::Player: {
			//	Create a new object of type Player
			Player* player = new Player();

			//	Push to type-specific players vector
			typeID = (SpecificID)this->players.push(player);

			//	Push to global objects vector
			globalID = (EntityID)this->objects.push(player);

			//	Set object's type and global IDs
			player->typeID = typeID;
			player->globalID = globalID;
			break;
		}
        case ObjectType::Enemy: {
            //	Create a new object of type Enemy
            Enemy* enemy = new Enemy();

			//	Push to type-specific enemies vector
            typeID = (SpecificID)this->enemies.push(enemy);

            //	Push to global objects vector
            globalID = (EntityID)this->objects.push(enemy);

            //	Set object's type and global IDs
            enemy->typeID = typeID;
            enemy->globalID = globalID;
            break;
        }
		case ObjectType::Object: {
			//	Create a new object of type Object
			Object* object = new Object(ObjectType::Object);

			//	TODO: Maybe change SmartVector's index return value? size_t is
			//	larger than uint32 (which is what SpecificID and EntityID are
			//	defined as)
			//	Push to type-specific base_objects vector
			typeID = (SpecificID)this->base_objects.push(object);

			//	Push to global objects vector
			globalID = (EntityID)this->objects.push(object);

            //	Set object's type and global IDs
            object->typeID = typeID;
            object->globalID = globalID;
            break;
        }
        default: {
            //	Create a new object of type Object
            Object* object = new Object(ObjectType::Object);

            //	TODO: Maybe change SmartVector's index return value? size_t is
            //	larger than uint32 (which is what SpecificID and EntityID are
            //	defined as)
            //	Push to type-specific base_objects vector
            typeID = (SpecificID)this->base_objects.push(object);

            //	Push to global objects vector
            globalID = (EntityID)this->objects.push(object);

            //	Set object's type and global IDs
            object->typeID = typeID;
            object->globalID = globalID;
            break;
        }
	}

	//	Return new object's specificID
	return typeID;
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

/*	SharedGameState generation	*/

std::vector<std::shared_ptr<SharedObject>> ObjectManager::toShared() {
	std::vector<std::shared_ptr<SharedObject>> shared;

	//	Fill shared SmartVector of SharedObjects
	for (int i = 0; i < this->objects.size(); i++) {
		Object* object = this->objects.get(i);

		if (object == nullptr) {
			//	Push empty object to SharedObject SmartVector
			shared.push_back(nullptr);
		}
		else {
			//	Create a SharedObject representation for this object and push it
			//	to the SharedObject SmartVector
			shared.push_back(std::make_shared<SharedObject>(object->toShared()));
		}
	}

	return shared;
}