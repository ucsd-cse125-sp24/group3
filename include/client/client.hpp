#pragma once

#include <iostream>
#include <ostream>
#include <utility>
#include <unordered_map>

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

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
    Client(boost::asio::io_service& io_service, GameConfig config);
    ~Client();
    int init();
    int start(boost::asio::io_context& context);
    void draw();
    void connectAndListen(std::string ip_addr);

private:
    void processClientInput();
    void processServerInput(boost::asio::io_context& context);

    SharedGameState gameState;
    //GameState gameState;

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

