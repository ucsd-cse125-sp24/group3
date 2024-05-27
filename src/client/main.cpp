#include <iostream>
#include <chrono>


#include <boost/asio/io_context.hpp>

#include "client/client.hpp"
#include "shared/utilities/rng.hpp"
#include "shared/utilities/config.hpp"
#include "client/audio/audiomanager.hpp"
#include "shared/utilities/root_path.hpp"

#include "shared/utilities/root_path.hpp"
#include "client/gui/img/img.hpp"

using namespace std::chrono_literals;

void error_callback(int error, const char* description) {
    // Print error.
    std::cerr << description << std::endl;
}

/**
 * @brief Sets various callbacks for the GLFWwindow. Uses the windowUserPointer object which
 * points to a Client object and wraps the member function calls with a lambda to pass in as
 * the static callback.
 * 
 * @param window 
 */
void set_callbacks(GLFWwindow* window) {
    // Set the error callback.
    glfwSetErrorCallback(error_callback);

    /* Set key callback */
    glfwSetKeyCallback(window, [](GLFWwindow* w, int key, int scancode, int action, int mods) {
        static_cast<Client*>(glfwGetWindowUserPointer(w))->keyCallback(w, key, scancode, action, mods);
    });

    /* Set mouse and cursor callbacks */
    glfwSetMouseButtonCallback(window, [](GLFWwindow* w, int button, int action, int mods) {
        static_cast<Client*>(glfwGetWindowUserPointer(w))->mouseButtonCallback(w, button, action, mods);
    });

    glfwSetCursorPosCallback(window, [](GLFWwindow* w, double xposIn, double yposIn) {
        static_cast<Client*>(glfwGetWindowUserPointer(w))->mouseCallback(w, xposIn, yposIn);
    });

    /* Set character callback for typing */
    glfwSetCharCallback(window, [](GLFWwindow* w, unsigned int codepoint) {
        static_cast<Client*>(glfwGetWindowUserPointer(w))->charCallback(w, codepoint);
    });
}

void set_opengl_settings(GLFWwindow* window) {
    // Enable depth buffering.
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);

    // Related to shaders and z value comparisons for the depth buffer.
    glDepthFunc(GL_LEQUAL);

    // Set polygon drawing mode to fill front and back of each polygon.
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

    // Sets initial background color.
    glClearColor(0.8f, 0.8f, 0.8f, 1.0f);

    // Set cursor position to (0, 0)
    glfwSetCursorPos(window, 0, 0);
}

int main(int argc, char* argv[])
{
    auto config = GameConfig::parse(argc, argv);
    boost::asio::io_context context;

    std::unique_ptr<Client> client(new Client(context, config));

    if (!client->init()) {
        std::cout << "client init failed" << std::endl;
        exit(EXIT_FAILURE);
    }
    
    GLFWwindow* window = client->getWindow();
    if (!window) exit(EXIT_FAILURE);

    // Set the user pointer for the glfw window
    glfwSetWindowUserPointer(window, client.get());
    // Setup callbacks.
    set_callbacks(window);
    // Setup OpenGL settings.
    set_opengl_settings(window);

    double lastTime = glfwGetTime();
    int nbFrames = 0;

    // Loop while GLFW window should stay open.
    while (!glfwWindowShouldClose(window)) {
        double currentTime = glfwGetTime();
        nbFrames++;
        if ( currentTime - lastTime >= 1.0 ){ // update fps every second
            client->curr_fps = nbFrames; 
            std::cout << client->curr_fps << "fps\n";
            nbFrames = 0;
            lastTime += 1.0;
        }

        // Main render display callback. Rendering of objects is done here.
        client->displayCallback();

        // Idle callback. Updating objects, etc. can be done here.
        client->idleCallback();
    }

    client->cleanup();

    exit(EXIT_SUCCESS);

    return 0;
}
