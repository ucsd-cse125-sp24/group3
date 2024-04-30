#include <iostream>
#include <chrono>

#include <boost/asio/io_context.hpp>
// #include <SDL.h>
#include <SFML/Audio.hpp>

#include "client/client.hpp"
#include "shared/utilities/rng.hpp"
#include "shared/utilities/config.hpp"
#include "client/sound.hpp"

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
    // glfwSetMouseButtonCallback(window, client.mouse_callback);
    // glfwSetCursorPosCallback(window, client.cursor_callback);
}

void set_opengl_settings() {
    // Enable depth buffering.
    glEnable(GL_DEPTH_TEST);

    // Related to shaders and z value comparisons for the depth buffer.
    glDepthFunc(GL_LEQUAL);

    // Set polygon drawing mode to fill front and back of each polygon.
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

    // Set clear color to black.
    glClearColor(0.8f, 0.8f, 0.8f, 1.0f);
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

    //if (SDL_Init(SDL_INIT_AUDIO) < 0)
    //    return 1;

    if (!client.init()) {
        exit(EXIT_FAILURE);
    }

    // Sound* CollisionSound = new Sound("./sounds/collide.wav");

    //CollisionSound->SetupDevice();

    //CollisionSound->PlaySound();

    //delete CollisionSound;

    GLFWwindow* window = client.getWindow();
    if (!window) exit(EXIT_FAILURE);

    // Setup callbacks.
    set_callbacks(window);
    // Setup OpenGL settings.
    set_opengl_settings();

    boost::filesystem::path soundFilepath = client.getRootPath() / "src/client/sounds/collide.wav";

    sf::SoundBuffer buffer;
    if (!buffer.loadFromFile(soundFilepath.string()))
        return -1;

    sf::Sound sound;
    sound.setBuffer(buffer);

    // Loop while GLFW window should stay open.
    while (!glfwWindowShouldClose(window)) {
        // Main render display callback. Rendering of objects is done here.
        client.displayCallback();

        // Idle callback. Updating objects, etc. can be done here.
        client.idleCallback(context);

        sound.play();
    }

    client.cleanup();

    // Destroy the window.
    glfwDestroyWindow(window);
    // Terminate GLFW.
    glfwTerminate();

    exit(EXIT_SUCCESS);

    return 0;
}
