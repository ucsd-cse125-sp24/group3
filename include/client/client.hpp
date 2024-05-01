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

class Client {

public:
    // Callbacks
    void displayCallback();
    void idleCallback(boost::asio::io_context& context);

    // Bound window callbacks
    static void keyCallback(GLFWwindow *window, int key, int scancode, int action, int mods);
    static void mouseCallback(GLFWwindow *window, double xposIn, double yposIn);


    // Getter / Setters
    GLFWwindow* getWindow() { return window; }

    Client(boost::asio::io_service& io_service, GameConfig config);
    ~Client();

    bool init();
    bool cleanup();

    void draw();
    void connectAndListen(std::string ip_addr);

private:
    void processClientInput();
    void processServerInput(boost::asio::io_context& context);

    SharedGameState gameState;

    float cubeMovementDelta = 0.05f;

    GLFWwindow *window;
    GLuint cubeShaderProgram;

    Camera *cam;

    // Flags
    static bool is_held_up;
    static bool is_held_down;
    static bool is_held_right;
    static bool is_held_left;

    static bool cam_is_held_up;
    static bool cam_is_held_down;
    static bool cam_is_held_right;
    static bool cam_is_held_left;

    static float mouse_xpos;
    static float mouse_ypos;

    GameConfig config;
    tcp::resolver resolver;
    tcp::socket socket;

    /// @brief Generate endpoints the client can connect to
    basic_resolver_results<class boost::asio::ip::tcp> endpoints;
    std::shared_ptr<Session> session;
};

