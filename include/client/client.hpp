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
#include "client/animation.hpp"
#include "client/animationmanager.hpp"
#include "client/bone.hpp"

#include "shared/game/sharedgamestate.hpp"
#include "shared/game/sharedobject.hpp"
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
    void mouseCallback(GLFWwindow* window, double xposIn, double yposIn);

    /**
     * @brief Callback which handles mouse button presses.
     * 
     * @param window The GLFWwindow being monitered.
     * @param button The mouse button pressed.
     * @param action One of GLFW_PRESS, GLFW_REPEAT, or GLFW_RELEASE representing the state of the button.
     * @param mods Any modifiers to the key pressed (i.e. shift, ctrl, alt, etc.).
     */
    void mouseButtonCallback(GLFWwindow* window, int button, int action, int mods);

    /**
     * @brief 
     * 
     * @param window 
     * @param codepoint 
     */
    void charCallback(GLFWwindow* window, unsigned int codepoint);

    /**
     * @brief Get a vec2 representing <width, height> of the window size.
     * 
     * @return glm::vec2 
     */
    static glm::vec2 getWindowSize();

    /**
     * @brief Get the time of last keystroke.
     * 
     * @return time_t 
     */
    static time_t getTimeOfLastKeystroke();

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
    bool connectAndListen(std::string ip_addr);

    AudioManager* getAudioManager();
    AnimationManager* getAnimManager() { return animManager; }

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

    /**
     * @brief Draw bounding box around a given SharedObject
     * only if the client.draw_bboxes field is set to true
     */
    void drawBbox(boost::optional<SharedObject> object);

    /* Current game state */
    SharedGameState gameState;

    /* Shader objects for various */
    std::shared_ptr<Shader> cube_shader; 
    std::shared_ptr<Shader> model_shader;
    std::shared_ptr<Shader> light_source_shader;
    std::shared_ptr<Shader> solid_surface_shader;

    /* Character models and lighting objects, might need to move to different classes later */
    std::unique_ptr<Model> cube_model;
    std::unique_ptr<Model> player_model;
    std::unique_ptr<Model> bear_model;
    std::unique_ptr<LightSource> light_source;

    GLFWwindow *window;

    /* GUI */
    friend class gui::GUI;
    gui::GUI gui;
    gui::GUIState gui_state;

    AudioManager* audioManager;
    AnimationManager* animManager;

    /* Camera object representing player's current position & orientation */
    std::unique_ptr<Camera> cam;

    /* Flags */
    static int window_width;
    static int window_height;

    static time_t time_of_last_keystroke;

    /* Key held flags */
    bool is_held_up = false;
    bool is_held_down = false;
    bool is_held_right = false;
    bool is_held_left = false;
    bool is_held_space = false;

    bool is_left_mouse_down = false;

    /* Mouse position coordinates */
    float mouse_xpos = 0.0f;
    float mouse_ypos = 0.0f;

    GameConfig config;
    tcp::resolver resolver;
    tcp::socket socket;

    LobbyFinder lobby_finder;

    /// @brief Generate endpoints the client can connect to
    basic_resolver_results<class boost::asio::ip::tcp> endpoints;
    std::shared_ptr<Session> session;
};

