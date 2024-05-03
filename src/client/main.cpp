#include <iostream>
#include <chrono>

#include <boost/asio/io_context.hpp>
#include <SFML/Audio.hpp>

#include "client/client.hpp"
#include "shared/utilities/rng.hpp"
#include "shared/utilities/config.hpp"
#include "client/audiomanager.hpp"

using namespace std::chrono_literals;

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
    // glfwSetMouseButtonCallback(window, Client::mouseCallback);
    glfwSetCursorPosCallback(window, Client::mouseCallback);
}

void set_opengl_settings(GLFWwindow* window) {
    // Enable depth buffering.
    glEnable(GL_DEPTH_TEST);

    // Related to shaders and z value comparisons for the depth buffer.
    glDepthFunc(GL_LEQUAL);

    // Set polygon drawing mode to fill front and back of each polygon.
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

    // Set clear color to black.
    glClearColor(0.8f, 0.8f, 0.8f, 1.0f);

    // Set cursor position to (0, 0)
    glfwSetCursorPos(window, 0, 0);
}

int main(int argc, char* argv[])
{
    auto config = GameConfig::parse(argc, argv);
    boost::asio::io_context context;
    LobbyFinder lobby_finder(context, config);
    Client client(context, config);
    if (config.client.lobby_discovery) {
        // TODO: once we have UI, there should be a way to connect based on
        // this. Right now, there isn't really a way to react to the information
        // the LobbyFinder is gathering.
        std::cerr << "Error: lobby discovery not enabled yet for client-side."
            << std::endl;
        std::exit(1);
        // lobby_finder.startSearching();
    } else {
        client.connectAndListen(config.network.server_ip);
    }

    if (!client.init()) {
        exit(EXIT_FAILURE);
    }

    GLFWwindow* window = client.getWindow();
    if (!window) exit(EXIT_FAILURE);

    // Setup callbacks.
    set_callbacks(window);
    // Setup OpenGL settings.
    set_opengl_settings(window);

    boost::filesystem::path soundFilepath = client.getRootPath() / "assets/sounds/piano.wav";

    sf::SoundBuffer buffer;
    if (!buffer.loadFromFile(soundFilepath.string()))
        return -1;

    sf::Sound sound;
    sound.setBuffer(buffer);

    sound.setLoop(true);

    sound.play();

    // Loop while GLFW window should stay open.
    while (!glfwWindowShouldClose(window)) {
        // Main render display callback. Rendering of objects is done here.
        client.displayCallback();

        // Idle callback. Updating objects, etc. can be done here.
        client.idleCallback(context);
    }

    client.cleanup();

    // Destroy the window.
    glfwDestroyWindow(window);
    // Terminate GLFW.
    glfwTerminate();

    exit(EXIT_SUCCESS);

    return 0;
}
