#pragma once

#include "client/core.hpp"

#include <iostream>
#include <ostream>
#include <utility>
#include <unordered_map>

#include <boost/asio/ip/tcp.hpp>
#include <boost/asio/io_service.hpp>

#include "client/cube.hpp"
#include "client/util.hpp"
#include "client/lobbyfinder.hpp"
#include "client/camera.hpp"

//#include "shared/game/gamestate.hpp"
#include "shared/game/sharedgamestate.hpp"
#include "shared/network/packet.hpp"
#include "shared/network/session.hpp"
#include "shared/utilities/config.hpp"

using namespace boost::asio::ip;

/**
 * @brief A clsas that represents the client for the game. Contains all local information,
 * including the SharedGameState, as well as callbacks for rendering objects and initializing 
 * glfw, glew, shaders, and windows.
 */
class Client {

public:
    /**
     * @brief Constructs a new Client object.
     * 
     * @param io_service 
     * @param config 
     */
    Client(boost::asio::io_service& io_service, GameConfig config);

    /**
     * @brief Destroys the Client object.
     */
    ~Client();

    /**
     * @brief Initializes glfw, glew, shaders, and the GLFWwindow.
     * 
     * @return true if all libraries and objects were initialized correctly, false otherwise
     */
    bool init();

    /**
     * @brief Frees all pointers, deletes the GLFWwindow, and terminates glfw.
     * 
     * @return true if all objects were cleaned up correclty, false otherwise
     */
    bool cleanup();

    // Callbacks
    /**
     * @brief Display callback which handles the rendering of all local objects. Abstracts
     * the logic of the main render loop. All logic relating to rendering and any glfw
     * or OpenGL calls should go in here.
     */
    void displayCallback();

    /**
     * @brief Callback which handles all updates to the local SharedGameState, and sends
     * events to the server based on any local inputs. All logic relating to state updates
     * shoud go in here.
     * 
     * @param context 
     */
    void idleCallback(boost::asio::io_context& context);

    /**
     * @brief Callback which handles keyboard inputs to the GLFWwindow. Binds to the GLFWwindow.
     * 
     * @param window The GLFWwindow being monitered.
     * @param key The key that is being pressed or released.
     * @param scancode A unique platform-specific code for each key.
     * @param action One of GLFW_PRESS, GLFW_REPEAT, or GLFW_RELEASE representing the state of the key.
     * @param mods Any modifiers to the key pressed (i.e. shift, ctrl, alt, etc.).
     */
    void keyCallback(GLFWwindow *window, int key, int scancode, int action, int mods);

    /**
     * @brief Callback which handles mouse cursor movement. Does not include mouse button presses.
     * 
     * @param window The GLFWwindow being monitered.
     * @param xposIn The current x-coordinate of the cursor.
     * @param yposIn The current y-coordinate of the cursor.
     */
    void mouseCallback(GLFWwindow *window, double xposIn, double yposIn);

    /**
     * @brief Callback which handles window resizing.
     * 
     * @param window The GLFWwindow being monitored.
     * @param width The new width of the window.
     * @param height The new height of the window.
     */
    void resizeCallback(GLFWwindow *window, int width, int height);

    // Getter / Setters
    /**
     * @brief Returns a pointer to the GLFWwindow object.
     * 
     * @return GLFWwindow* The GLFWwindow being used in the Client.
     */
    GLFWwindow* getWindow() { return window; }

    /**
     * @brief Creates and connects to a Session at the specified IP address.
     * 
     * @param ip_addr 
     */
    void connectAndListen(std::string ip_addr);

private:
    /**
     * @brief Processes all data received from the server and updates the SharedGameState.
     * 
     * @param context
     */
    void processServerInput(boost::asio::io_context& context);

    /**
     * @brief Draws all objects in the SharedGameState.
     */
    void draw();

    SharedGameState gameState;

    GLFWwindow *window;
    int width;
    int height;

    GLuint cubeShaderProgram;

    std::unique_ptr<Camera> cam;

    bool cam_is_held_up = false;
    bool cam_is_held_down = false;
    bool cam_is_held_right = false;
    bool cam_is_held_left = false;

    float mouse_xpos = 0.0f;
    float mouse_ypos = 0.0f;

    GameConfig config;
    tcp::resolver resolver;
    tcp::socket socket;

    /// @brief Generate endpoints the client can connect to
    basic_resolver_results<class boost::asio::ip::tcp> endpoints;
    std::shared_ptr<Session> session;
};

