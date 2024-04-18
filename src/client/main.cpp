#include <iostream>

#include "client/client.hpp"
#include "shared/utilities/rng.hpp"

void error_callback(int error, const char* description) {
    // Print error.
    std::cerr << description << std::endl;
}

void set_callbacks(GLFWwindow* window) {
    // Set the error callback.
    glfwSetErrorCallback(error_callback);
    // Set the window resize callback.
    // glfwSetWindowSizeCallback(window, client.resizeCallback);

    // Set the key callback.
    glfwSetKeyCallback(window, Client::keyCallback);

    // Set the mouse and cursor callbacks
    // glfwSetMouseButtonCallback(window, client.mouse_callback);
    // glfwSetCursorPosCallback(window, client.cursor_callback);
}

void set_opengl_settings() {
    // Enable depth buffering.
    glEnable(GL_DEPTH_TEST);
    // Related to shaders and z value comparisons for the depth buffer.
    glDepthFunc(GL_LEQUAL);
    // Set polygon drawing mode to fill front and back of each polygon.
    // glColor3f(1.0f, 255, 255);
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    // Set clear color to black.
    glClearColor(0.0, 0.0, 0.0, 0.0);
}


int main() {
    Client::init();

    std::cout << "I am a client!\n"; 

    GLFWwindow* window = Client::getWindow();
    if (!window) exit(EXIT_FAILURE);

    // Setup callbacks.
    set_callbacks(window);
    // Setup OpenGL settings.
    set_opengl_settings();

    // Initialize the shader program; exit if initialization fails.
    // if (!Window::initializeProgram()) exit(EXIT_FAILURE);
    // Initialize objects/pointers for rendering; exit if initialization fails.
    // if (!Window::initializeObjects(argc, argv)) exit(EXIT_FAILURE);

    // Loop while GLFW window should stay open.
    while (!glfwWindowShouldClose(window)) {
        // Main render display callback. Rendering of objects is done here.
        Client::displayCallback();

        // Idle callback. Updating objects, etc. can be done here.
        Client::idleCallback();
    }

    // Window::cleanUp();
    // Destroy the window.
    glfwDestroyWindow(window);
    // Terminate GLFW.
    glfwTerminate();

    exit(EXIT_SUCCESS);
}
