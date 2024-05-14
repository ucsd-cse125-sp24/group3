#pragma once

#include <memory>

#include "server/game/object.hpp"
#include "server/game/player.hpp"
#include "server/game/enemy.hpp"
#include "server/game/solidsurface.hpp"
#include "server/game/torchlight.hpp"
#include "shared/utilities/smartvector.hpp"

// forward declarations to use pointers
class Trap; 
class Projectile;
class Item;


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
	 * @return the SpecificID of the newly created object
	 */
	SpecificID createObject(Object* object);

	/**
	 * @brief Attempts to remove an object with the given EntityID.
	 * 
	 * @param globalID EntityID of the object to remove.
	 * @return true if the object was successfully removed and false otherwise.
	 */
	bool removeObject(EntityID globalID);

	/**
	 * @brief Attempts to remove an object pointed to by the pointer pointed to
	 * by the given Object double pointer.
	 * If successful, it will set the given Object pointer to nullptr. (use to
	 * avoid dangling Object pointers)
	 * @param object_dbl_ptr Double pointer to the object to remove
	 * @return true if the object was successfully removed and false otherwise.
	 */
	bool removeObject(Object** object_dbl_ptr);

	/**
	 * @brief Attempts to retrieve the object with the given EntityID.
	 * @param globalID EntityID of the object to retrieve
	 * @return A pointer to the object with the given EntityID or nullptr if
	 * none exists.
	 */
	Object* getObject(EntityID globalID);

	/*	SpecificID object getters by type	*/

	/**
	 * @brief Attempts to retrieve the Object with the given SpecificID.
	 * @param base_objectID SpecificID of the Object to retrieve
	 * @return A pointer to the Object with the given SpecificID or nullptr if
	 * none exists.
	 */
	Object* getBaseObject(SpecificID base_objectID);

	/**
	 * @brief Attempts to retrieve the Item with the given SpecificID.
	 * @param itemID SpecificID of the Item to retrieve
	 * @return A pointer to the Item with the given SpecificID or nullptr if
	 * none exists.
	 */
	Item* getItem(SpecificID itemID);

	/**
	 * @brief Attempts to retrieve the SolidSurface with the given SpecificID.
	 * @param surfaceID SpecificID of the SolidSurface to retrieve
	 * @return A pointer to the SolidSurface with the given SpecificID or
	 * nullptr if none exists.
	 */
	SolidSurface* getSolidSurface(SpecificID surfaceID);

	/**
	 * @brief Attempts to retrieve the Player with the given SpecificID.
	 * @param playerID SpecificID of the Player to retrieve
	 * @return A pointer to the Player with the given SpecificID or nullptr if
	 * none exists.
	 */
	Player* getPlayer(SpecificID playerID);

	/**
	 * @brief Attempts to retrieve the Enemy with the given SpecificID.
	 * @param enemyID SpecificID of the Enemy to retrieve
	 * @return A pointer 
	 */
	Enemy* getEnemy(SpecificID enemyID);

	/**
	 * @brief Attempts to retrieve the Torchlight with the given SpecificID.
	 * @param torchlightID SpecificID of the Torchlight to retrieve
	 * @return A pointer 
	 */
	Torchlight* getTorchlight(SpecificID torchlightID);

    /**
	 * @brief Attempts to retrieve the Trap with the given SpecificID.
	 * @param trapID lSpecificID of the Trap to retrieve
	 * @return A pointer 
	 */
	Trap* getTrap(SpecificID trapID);

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

	/**
	 * @brief Get a list of all SolidSurfaces in this game instance at the
	 * current timestep.
	 * @return SmartVector of SolidSurface pointers of all SolidSurface objects
	 * in the game instance.
	 */
	SmartVector<SolidSurface*> getSolidSurfaces();

	/**
	 * @brief Get a list of all Players in this game instance at the current
	 * timestep.
	 * @return SmartVector of Player pointers of all Player objects in the game
	 * instance.
	 */
	SmartVector<Player*> getPlayers();

	/**
	 * @brief Get a list of all Enemies in this game instance at the current
	 * timestep.
	 * @return SmartVector of Enemy pointers of all Enemy objects in the game
	 * instance.
	 */
	SmartVector<Enemy*> getEnemies();

	/**
	 * @brief Get a list of all Traps in this game instance at the current
	 * timestep.
	 * @return SmartVector of Trap pointers of all Trap objects in the game
	 * instance
	 */
	SmartVector<Trap*> getTraps();

	/**
	 * @brief Get a list of all Projectiles in this game instance at the current
	 * timestep.
	 * @return SmartVector of Projectile pointers of all Projectile objects in the game
	 * instance
	 */
	SmartVector<Projectile*> getProjectiles();

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
	SmartVector<Object *> objects;

	/*	Type-specific object smart vectors	*/
	
	/**
	 * @brief SmartVector of Object pointers to all objects whose ObjectType is
	 * ObjectType::Object.
	 */
	SmartVector<Object *> base_objects;

	/**
	 * @brief SmartVector of Item pointers to all Item objects.
	 */
	SmartVector<Item *> items;

	/**
	 * @brief SmartVector of SolidSurface pointers to all SolidSurface objects.
	 */
	SmartVector<SolidSurface *> solid_surfaces;

	/**
	 * @brief SmartVector of Player pointers to all objects whose ObjectType is
	 * ObjectType::Player.
	 */
	SmartVector<Player *> players;

	/**
	 * @brief SmartVector of Enemy pointers to all objects whose ObjectType is
	 * ObjectType::Enemy.
	 */
	SmartVector<Enemy *> enemies;

	/**
	 * @brief SmartVector of TorchLight pointers to all objects whose ObjectType is
	 * ObjectType::Enemy.
	 */
	SmartVector<Torchlight *> torchlights;

    /**
	 * @brief SmartVector of Trap pointers to all objects whose ObjectType is
	 * ObjectType::Trap.
	 */
	SmartVector<Trap *> traps;

	/**
	 * @brief SmartVector of projectile pointers to all objects whose ObjectType is
	 * ObjectType::Projectile.
	 */
	SmartVector<Projectile *> projectiles;
};