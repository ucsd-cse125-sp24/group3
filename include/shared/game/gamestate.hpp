#pragma once

#include "GameLogic/Object.hpp"

#include <string>

class GameState {
public:
    GameState();

    void updateState();

    void createObject(Object * object);

    Object* removeObject();

private:
    /**
     *  Vector of all objects in this game instance.
     */
    std::vector<Object> objects;

    /**
     *  Time step length in milliseconds.
     */
    unsigned int time_step_len;

    /**
     *  Current time step (starts at 0)
     */
    unsigned int time_step;
};