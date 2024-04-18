#pragma once

#include "client/core.hpp"

#include <iostream>
#include <ostream>

#include "client/cube.hpp"
#include "client/util.hpp"

class Client {

public:

    // init
    static int init();
    static int cleanup();

    // Callbacks
    static void displayCallback();
    static void idleCallback();
    static void keyCallback(GLFWwindow *window, int key, int scancode, int action, int mods);

    // Getter / Setters
    static GLFWwindow* getWindow() { return window; }

private:
    // static void processInput();

    static float cubeMovementDelta;
    static Cube* cube;
    static GLFWwindow *window;
    static GLuint shaderProgram;

    // Flags
    static bool is_held_up;
    static bool is_held_down;
    static bool is_held_right;
    static bool is_held_left;

};

