#pragma once

#include "client/core.hpp"

#include <iostream>
#include <ostream>
#include <utility>
#include <unordered_map>
#include <chrono>

#include <boost/asio/ip/tcp.hpp>
#include <boost/asio/io_service.hpp>
#include <boost/filesystem.hpp>

#include "client/cube.hpp"
#include "client/util.hpp"
#include "client/lobbyfinder.hpp"
#include "client/gui/gui.hpp"
#include "client/camera.hpp"

//#include "shared/game/gamestate.hpp"
#include "shared/game/sharedgamestate.hpp"
#include "shared/network/packet.hpp"
#include "shared/network/session.hpp"
#include "shared/utilities/config.hpp"

#define WINDOW_WIDTH  1500
#define WINDOW_HEIGHT 1000

// position something a "frac" of the way across the screen
// e.g. WIDTH_FRAC(4) -> a fourth of the way from the left
//      HEIGHT_FRAC(3) -> a third of the way from the bottom
#define FRAC_WINDOW_WIDTH(num, denom) WINDOW_WIDTH * static_cast<float>(num) / static_cast<float>(denom)
#define FRAC_WINDOW_HEIGHT(num, denom) WINDOW_HEIGHT * static_cast<float>(num) / static_cast<float>(denom)

using namespace boost::asio::ip;

class Client {

public:
    // Callbacks
    void displayCallback();

    void idleCallback(boost::asio::io_context& context);
    void handleKeys(int eid, int keyType, bool keyHeld, bool *eventSent, glm::vec3 movement = glm::vec3(0.0f));

    // Bound window callbacks
    static void keyCallback(GLFWwindow *window, int key, int scancode, int action, int mods);
    static void mouseCallback(GLFWwindow* window, double xposIn, double yposIn);
    static void mouseButtonCallback(GLFWwindow* window, int button, int action, int mods);
    static void charCallback(GLFWwindow* window, unsigned int codepoint);
    static time_t getTimeOfLastKeystroke();


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

    friend class gui::GUI;
    gui::GUI gui;
    gui::GUIState gui_state;
    Camera *cam;

    // Flags
    static bool is_held_up;
    static bool is_held_down;
    static bool is_held_right;
    static bool is_held_left;
    static bool is_held_space;
    static bool is_held_shift;

    static bool cam_is_held_up;
    static bool cam_is_held_down;
    static bool cam_is_held_right;
    static bool cam_is_held_left;

    static bool is_left_mouse_down;
    static bool is_click_available;
    static float mouse_xpos;
    static float mouse_ypos;

    static time_t time_of_last_keystroke;

    GameConfig config;
    tcp::resolver resolver;
    tcp::socket socket;

    LobbyFinder lobby_finder;

    /// @brief Generate endpoints the client can connect to
    basic_resolver_results<class boost::asio::ip::tcp> endpoints;
    std::shared_ptr<Session> session;

    boost::filesystem::path root_path;
};

