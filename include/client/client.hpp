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
#include "client/lightsource.hpp"
#include "client/shader.hpp"
#include "client/model.hpp"
#include "client/util.hpp"
#include "client/lobbyfinder.hpp"
#include "client/gui/gui.hpp"
#include "client/camera.hpp"
#include "client/audiomanager.hpp"

//#include "shared/game/gamestate.hpp"
#include "shared/game/sharedgamestate.hpp"
#include "shared/network/packet.hpp"
#include "shared/network/session.hpp"
#include "shared/utilities/config.hpp"

#define WINDOW_WIDTH Client::getWindowSize().x
#define WINDOW_HEIGHT Client::getWindowSize().y

// position something a "frac" of the way across the screen
// e.g. WIDTH_FRAC(1, 4) -> a fourth of the way from the left
//      HEIGHT_FRAC(2, 3) -> two thirds of the way from the bottom
#define FRAC_WINDOW_WIDTH(num, denom) Client::window_width * static_cast<float>(num) / static_cast<float>(denom)
#define FRAC_WINDOW_HEIGHT(num, denom) Client::window_height * static_cast<float>(num) / static_cast<float>(denom)

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

    static glm::vec2 getWindowSize();

    static time_t getTimeOfLastKeystroke();

    // Getter / Setters
    GLFWwindow* getWindow() { return window; }

    Client(boost::asio::io_service& io_service, GameConfig config);
    ~Client();

    bool init();
    bool cleanup();

    void draw();
    void connectAndListen(std::string ip_addr);

    AudioManager* getAudioManager();

private:
    void processClientInput();
    void processServerInput(boost::asio::io_context& context);

    SharedGameState gameState;

    std::shared_ptr<Shader> cube_shader; 
    std::shared_ptr<Shader> model_shader;
    std::shared_ptr<Shader> light_source_shader;

    std::unique_ptr<Model> player_model;
    std::unique_ptr<Model> bear_model;
    std::unique_ptr<LightSource> light_source;

    float playerMovementDelta = 0.05f;

    GLFWwindow *window;

    friend class gui::GUI;
    gui::GUI gui;
    gui::GUIState gui_state;
    Camera *cam;

    AudioManager* audioManager;

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

    static int window_width;
    static int window_height;

    static time_t time_of_last_keystroke;

    GameConfig config;
    tcp::resolver resolver;
    tcp::socket socket;

    LobbyFinder lobby_finder;

    /// @brief Generate endpoints the client can connect to
    basic_resolver_results<class boost::asio::ip::tcp> endpoints;
    std::shared_ptr<Session> session;
};

