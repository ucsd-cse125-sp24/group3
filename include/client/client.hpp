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

    GLuint cubeShaderProgram;
    float cubeMovementDelta = 0.05f;

    GLFWwindow *window;

    // Flags
    static bool is_held_up;
    static bool is_held_down;
    static bool is_held_right;
    static bool is_held_left;

    GameConfig config;
    tcp::resolver resolver;
    tcp::socket socket;

    /// @brief Generate endpoints the client can connect to
    basic_resolver_results<class boost::asio::ip::tcp> endpoints;
    std::shared_ptr<Session> session;
};

