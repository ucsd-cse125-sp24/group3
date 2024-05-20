#pragma once 

#include <cstdint>

/**
 * @brief Global Object ID (when the server or client references an object with
 * a particular EntityID, they both refer to the same object)
 */
using EntityID = uint32_t;


/** 
 * @brief Object ID within an object type-specific SmartVector (used by
 * ServerGameState's ObjectManager)
 */
using SpecificID = uint32_t;

/**
 * @brief Object ID within an movable SmartVector (used by
 * ServerGameState's ObjectManager)
 */
using MovableID = uint32_t;