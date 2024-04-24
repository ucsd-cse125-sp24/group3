#include "server/game/objectmanager.hpp"

/*	Constructors and Destructors	*/

ObjectManager::ObjectManager() {
	//	Initialize global SmartVector
	this->objects = SmartVector<Object*>();

	//	Initialize type-specific SmartVectors
	this->base_objects = SmartVector<Object*>();
}

ObjectManager::~ObjectManager() {
	//	TODO: Delete all allocated objects
}

/*	Object CRUD methods	*/

EntityID ObjectManager::createObject(ObjectType type) {
	//	Create a new object with the given type
	EntityID globalID;
	SpecificID typeID;

	switch (type) {
	case ObjectType::Object:
		//	Create a new object of type Object
		Object* object = new Object();

		//	TODO: Maybe change SmartVector's index return value? size_t is
		//	larger than uint32 (which is what SpecificID and EntityID are
		//	defined as)
		//	Push to type-specific base_objects vector
		typeID = (SpecificID) this->base_objects.push(object);

		//	Push to global objects vector
		globalID = (EntityID) this->objects.push(object);

		//	Set object's type and global IDs
		object->typeID = typeID;
		object->globalID = globalID;
		break;
	}

	//	Return new object's global EntityID
	return globalID;
}

bool ObjectManager::removeObject(EntityID globalID) {
	//	Check that the given object exists
	Object** ptrToPtr = this->objects.get(globalID);

	if (ptrToPtr == nullptr) {
		//	Object with the given index doesn't exist
		return false;
	}

	Object* object = *ptrToPtr;

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

Object* ObjectManager::getObject(EntityID globalID) {
	Object** ptrToPtr = this->objects.get(globalID);

	if (ptrToPtr == nullptr) {
		return nullptr;
	}

	return *ptrToPtr;
}

SmartVector<Object*> ObjectManager::getObjects() {
	return this->objects;
}

/*	SharedGameState generation	*/

SmartVector<SharedObject> ObjectManager::toShared() {
	SmartVector<SharedObject> shared;

	//	Fill shared SmartVector of SharedObjects
	for (int i = 0; i < this->objects.size(); i++) {
		Object** ptrToPtr = this->objects.get(i);

		if (ptrToPtr == nullptr) {
			//	Push empty object to SharedObject SmartVector
			shared.pushEmpty();
		}
		else {
			Object* object = *ptrToPtr;

			//	Create a SharedObject representation for this object and push it
			//	to the SharedObject SmartVector
			shared.push(object->toShared());
		}
	}

	return shared;
}