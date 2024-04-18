#include "client/client.hpp"
#include <GLFW/glfw3.h>

float Client::cubeMovementDelta = 0.05;
Cube* Client::cube;
GLFWwindow* Client::window;
GLuint Client::shaderProgram;

// Flags
bool Client::is_held_up = false;
bool Client::is_held_down = false;
bool Client::is_held_right = false;
bool Client::is_held_left = false;

int Client::init() {
    /* Initialize glfw library */
    if (!glfwInit())
        return -1;

    /* Create a windowed mode window and its OpenGL context */
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    window = glfwCreateWindow(640, 480, "Hello World", NULL, NULL);
    if (!window)
    {
        glfwTerminate();
        return -1;
    }

    /* Make the window's context current */
    glfwMakeContextCurrent(window);

    GLenum err = glewInit() ; 
    if (GLEW_OK != err) { 
        std::cerr << "Error: " << glewGetString(err) << std::endl; 
    } 

    std::cout << "shader version: " << glGetString(GL_SHADING_LANGUAGE_VERSION) << std::endl;
    std::cout << "shader version: " << glGetString(GL_VERSION) << std::endl;

    /* Load shader programs */
    std::cout << "loading shader" << std::endl;
    shaderProgram = LoadShaders("../src/client/shaders/shader.vert", "../src/client/shaders/shader.frag");

    // Check the shader program.
    if (!shaderProgram) {
        std::cerr << "Failed to initialize shader program" << std::endl;
        return false;
    }

    cube = new Cube();

    return 0;
}

int Client::cleanup() {
    glDeleteProgram(shaderProgram);
    delete cube;
    return 0;
}

// Handles all rendering
void Client::displayCallback() {
    // processInput();
    /* Render here */
    glClearColor(0.8f, 0.8f, 0.8f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    /* Swap front and back buffers */
    cube->draw(shaderProgram);

    /* Poll for and process events */
    glfwPollEvents();
    glfwSwapBuffers(window);
}

// Handle any updates, 
void Client::idleCallback() {
    if(is_held_right)
        cube->update_delta(glm::vec3(cubeMovementDelta, 0.0f, 0.0f));
    if(is_held_left)
        cube->update_delta(glm::vec3(-cubeMovementDelta, 0.0f, 0.0f));
    if(is_held_up)
        cube->update_delta(glm::vec3(0.0f, cubeMovementDelta, 0.0f));
    if(is_held_down)
        cube->update_delta(glm::vec3(0.0f, -cubeMovementDelta, 0.0f));
}

// void Client::processInput() {
//     if(glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
//         glfwSetWindowShouldClose(window, true);
//     if(glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS)
//         glfwSetWindowShouldClose(window, true);
//     if(glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS)
//         cube->update_delta(glm::vec3(cubeMovementDelta, 0.0f, 0.0f));
//     if(glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS)
//         cube->update_delta(glm::vec3(-cubeMovementDelta, 0.0f, 0.0f));
//     if(glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS)
//         cube->update_delta(glm::vec3(0.0f, cubeMovementDelta, 0.0f));
//     if(glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS)
//         cube->update_delta(glm::vec3(0.0f, -cubeMovementDelta, 0.0f));
// }

// callbacks - for Interaction
void Client::keyCallback(GLFWwindow *window, int key, int scancode, int action, int mods) {
  // Check for a key press.
    if (action == GLFW_PRESS) {
        switch (key) {
        case GLFW_KEY_ESCAPE:
            // Close the window. This causes the program to also terminate.
            glfwSetWindowShouldClose(window, GL_TRUE);
            break;

        case GLFW_KEY_DOWN:
            is_held_down = true;
            break;

        case GLFW_KEY_UP:
            is_held_up = true;
            break;

        case GLFW_KEY_LEFT:
            is_held_left = true;
            break;

        case GLFW_KEY_RIGHT:
            is_held_right = true;
            break;

        default:
            break;
        }
    }

    if (action == GLFW_RELEASE) {
        switch (key) {
        case GLFW_KEY_DOWN:
            is_held_down = false;
            break;

        case GLFW_KEY_UP:
            is_held_up = false;
            break;

        case GLFW_KEY_LEFT:
            is_held_left = false;
            break;

        case GLFW_KEY_RIGHT:
            is_held_right = false;
            break;

        default:
            break;
        }
    }
}
