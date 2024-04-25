#pragma once

#include <memory>

#include "server/game/object.hpp"
#include "server/game/item.hpp"

#include "shared/utilities/smartvector.hpp"

class ObjectManager {
public:
	ObjectManager();
	~ObjectManager();

	/*	Object CRUD methods	*/

	/**
	 * @brief Creates a new object with the specified type.
	 * 
	 * @note This method assigns a unique global EntityID and a type-specific
	 * SpecificID to the newly created object (NOTE: the Object constructor does
	 * NOT do this!)
	 * 
	 * @param type the type of object to create
	 * @return the EntityID of the newly created object
	 */
	EntityID createObject(ObjectType type);

	/**
	 * @brief Attempts to remove an object with the given EntityID.
	 * 
	 * @param globalID EntityID of the object to remove.
	 * @return true if the object was successfully removed and false otherwise.
	 */
	bool removeObject(EntityID globalID);

	/**
	 * @brief Attempts to retrieve the object with the given EntityID.
	 * @param globalID EntityID of the object to retrieve
	 * @return A pointer to the object with the given EntityID or nullptr if
	 * none exists.
	 */
	Object* getObject(EntityID globalID);

	/**
	 * @brief Get a list of all objects in this game instance at the current
	 * timestep.
	 * @return SmartVector of Object pointers of all objects in the game
	 * instance.
	 */
	SmartVector<Object*> getObjects();

	/**
	 * @brief Get a list of all items in this game instance at the current
	 * timestep.
	 * @return SmartVector of Item pointers of all items in the game
	 * instance.
	 */
	SmartVector<Item*> getItems();

	/*	SharedGameState generation	*/
	
	/**
	 * @brief Generates a list of SharedObjects that corresponds to all objects
	 * in the game instance.
	 * @return Returns a std::vector<SharedObject> that corresponds to all
	 * objects in the game instance.
	 */
	std::vector<std::shared_ptr<SharedObject>> toShared();

private:
	/*
	 * Note on how Objects are stored:
	 * 
	 * The ObjectManager class maintains a SmartVector<Object *> objects vector
	 * that holds pointers to all Objects in the game instance at the current
	 * timestep.
	 * 
	 * The global objects SmartVector is indexed by each object's global
	 * EntityID; that is, if an object's EntityID is x, then a pointer to that
	 * object is stored in index x of the objects SmartVector.
	 * 
	 * The Objects are dynamically allocated on the heap when created, meaning
	 * there is no guaranteed contiguous object storage in memory.
	 * 
	 * Type-specific vectors are available for convenience and contain duplicate
	 * pointers; e.g., a SmartVector<Item *> will contain Item * pointers to all
	 * objects whose types derive from Item.
	 * 
	 * The type-specific vectors are indexed by their objects' type-specific
	 * SpecificID; that is, if an object is stored in SmartVector<Item *> items
	 * and its SpecificID is x, then at index x of items is a pointer to that
	 * object.
	 * 
	 * As each object has only one type-specific SpecificID, each object may
	 * only be stored in one type-specific SmartVector.
	 */

	/**
	 * @brief SmartVector of Object pointers to all objects in the current 
	 * timestep of this game instance.
	 * 
	 * The objects smart vector is indexed by each Object's global EntityID; 
	 * that is, the Object pointer at index x points to the Object with global 
	 * EntityID x.
	 */
	SmartVector<Object*> objects;

	/*	Type-specific object smart vectors	*/
	
	/**
	 * @brief SmartVector of Object pointers to all objects whose ObjectType is
	 * ObjectType::Object.
	 */
	SmartVector<Object*> base_objects;

	/**
	 * @brief SmartVector of Item pointers to all items ObjectType::Item.
	 */
	SmartVector<Item*> base_items;
};