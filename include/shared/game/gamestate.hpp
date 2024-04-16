#pragma once

#include "shared/game/GameLogic/Object.hpp"
#include <vector>
#include <string>

//	Constants
#define	FIRST_TIMESTEP	0
#define	TIMESTEP_LEN	30

class GameState {
public:
    /**
     * @brief Default GameState constructor. Creates a GameState instance with
     * an empty world (no objects) with default timestep length.
     */
    GameState() : GameState(FIRST_TIMESTEP, TIMESTEP_LEN) {}

    /**
     * @brief GameState constructor that sets the current timestep and the
     * timestep length to the given arguments.
     *
     * @param timestep Current timestep
     * @param timestep_length Timestep length
     */
    GameState(unsigned int timestep, unsigned int timestep_length);

    /**
     * @brief Updates this GameState instance from the current timestep to the
     * next one.
     */
    void update();

    /**
     * @brief Creates a new Object and places it in the world (adds it to the
     * objects vector). The object's position is at the origin and has 0 vectors
     * for velocity and acceleration.
     *
     * @return a pointer to the newly added object.
     */
    Object* createObject();

    /**
     * @brief Attempt to remove an object with the given id.
     * @param id ID of the object to remove from this game instance.
     * @return true if successfully removed the object and false otherwise.
     */
    bool removeObject(unsigned int id);

    /**
     * @brief Attempts to retrieve the object with the given id.
     * @param id ID of the object to retrieve
     * @return a pointer to the object with the given id, or nullptr if none
     * exists.
     */
    Object* getObject(unsigned int id);

    std::string to_string();
    unsigned int getTimestep() { return this->timestep; }
    unsigned int getTimestepLength() { return this->timestep_length; }

private:
    /**
     *  Vector of all objects in the current timestep of this game instance.
     */
    std::vector<Object> objects;

    /**
     *  Timestep length in milliseconds.
     */
    unsigned int timestep_length;

    /**
     *  Current timestep (starts at 0)
     */
    unsigned int timestep;
};