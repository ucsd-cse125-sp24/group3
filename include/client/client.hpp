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

#include "shared/game/gamestate.hpp"
#include "shared/network/packet.hpp"
#include "shared/network/session.hpp"
#include "shared/utilities/config.hpp"

using namespace boost::asio::ip;

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

    Client(boost::asio::io_service& io_service, GameConfig config);
    ~Client();
    int init();
    int start(boost::asio::io_context& context);
    void draw();
    void connectAndListen(std::string ip_addr);

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

    void processClientInput();
    void processServerInput(boost::asio::io_context& context);

    GameState gameState;

    float cubeMovementDelta = 0.05f;

    GLFWwindow *window;
    GLuint shaderProgram;

    GameConfig config;
    tcp::resolver resolver;
    tcp::socket socket;

    /// @brief Generate endpoints the client can connect to
    basic_resolver_results<class boost::asio::ip::tcp> endpoints;
    std::shared_ptr<Session> session;
};

