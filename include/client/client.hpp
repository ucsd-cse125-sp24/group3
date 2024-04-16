#pragma once

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

#include <iostream>
#include <ostream>

#include "client/cube.hpp"
#include "client/util.hpp"

class Client {
public:
    Client();
    ~Client();
    int init();
    int start();

private:
    void processInput();

    Cube* cube;
    float cubeMovementDelta = 0.05f;

    GLFWwindow *window;
    GLuint shaderProgram;
};

