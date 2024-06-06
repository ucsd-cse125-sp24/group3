#include "client/core.hpp"

#include <iostream>
#include <ostream>
#include <utility>
#include <unordered_map>
#include <chrono>
#include <map>

#include <boost/asio/ip/tcp.hpp>
#include <boost/asio/io_service.hpp>
#include <boost/filesystem.hpp>

#include "client/shader.hpp"
#include "client/model.hpp"
#include "client/util.hpp"
#include "client/lobbyfinder.hpp"
#include "client/gui/gui.hpp"
#include "client/camera.hpp"
#include "client/audio/audiomanager.hpp"
#include "client/constants.hpp"
#include "client/animation.hpp"
#include "client/animationmanager.hpp"
#include "client/bone.hpp"

#include "shared/game/sharedgamestate.hpp"
#include "shared/game/sharedobject.hpp"
#include "shared/network/packet.hpp"
#include "shared/network/session.hpp"
#include "shared/utilities/config.hpp"
#include "shared/utilities/constants.hpp"
#include "shared/utilities/typedefs.hpp"

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
     */
    void idleCallback();

    /**
     * @brief sends all queued packets to server
     */
    void sendPacketsToServer();

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
     * @brief Callback which handles scrolling movement. 
     *
     * @param window The GLFWwindow being monitered.
     * @param xposIn The current scroll up value.
     * @param yposIn The current scroll down value.
     */
    void scrollCallback(GLFWwindow* window, double xposIn, double yposIn);

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
    bool connect(std::string ip_addr);

    /**
     * @brief get the reference to the Audio manager
     */
    AudioManager* getAudioManager();

    /**
     * @brief get the reference to the Animation manager
     */
    AnimationManager* getAnimManager() { return animManager; }

    /**
     * @brief the current position in the world the player is looking at
     */
    void setWorldPos();

    /**
     * @brief Send down a trap event to the server 
     * 
     * @param hover boolean to indicate the DM is only hovering and has not placed yet
     * @param place boolean to indicate the DM would like to place a type
     * @param trapType ModelType to indicate the type of the trap the DM wants to place
     */
    void sendTrapEvent(bool hover, bool place, ModelType trapType);

    int curr_fps;
private:
    /**
     * @brief Processes all data received from the server and updates the SharedGameState.
     * 
     * @param allow_defer whether or not you are allowed to defer packets until the next frame
     * IMPORTANT: this is a performance optimization for more unstable networks, but it must
     * be set to false until the ServerAssignEID packet has been received because then it
     * guarantees the game has been fully loaded before trying to render things
     */
    void processServerInput(bool allow_defer);

    GLuint gBuffer;
    GLuint gPosition, gNormal, gAlbedoSpec;
    GLuint quadVAO = 0;
    GLuint quadVBO;
    GLuint cubeVAO = 0;
    GLuint cubeVBO = 0;

    void configureGBuffer();

    /**
     * @brief Draws all objects in the SharedGameState.
     */
    void draw();

    /**
     * @brief Goes through all objects and render them to 
     * G-Buffer. Does not handle lighting. Only stores
     * each object's positions, normals and textures.
     */
    void geometryPass();

    /**
     * @brief Loop through visible pixels and render 
     * lighting effects for the object at each pixel.
     */
    void lightingPass();

    /**
     * @brief Draw bounding box around a given SharedObject
     * only if the client.draw_bboxes field is set to true
     */
    void drawBbox(boost::optional<SharedObject> object);

    /* Current game state */
    SharedGameState gameState;

    /* Stuff for the intro cutscene, contains a whole other SharedGameState */
    std::optional<LoadIntroCutsceneEvent> intro_cutscene;

    /* Shader objects for various */
    std::shared_ptr<Shader> deferred_geometry_shader;
    std::shared_ptr<Shader> deferred_lighting_shader;
    std::shared_ptr<Shader> dm_deferred_lighting_shader;
    std::shared_ptr<Shader> deferred_light_box_shader;

    /* Character models and lighting objects, might need to move to different classes later */
    std::unique_ptr<Model> player_model;
    std::unique_ptr<Model> bear_model;
    std::unique_ptr<Model> torchlight_model;
    std::unique_ptr<Model> wall_model;
    std::unique_ptr<Model> pillar_model;
    std::unique_ptr<Model> sungod_model;
    std::unique_ptr<Model> slime_model;
    std::unique_ptr<Model> python_model;
    std::unique_ptr<Model> item_model;
    std::unique_ptr<Model> spike_trap_model;
    std::unique_ptr<Model> orb_model;
    std::unique_ptr<Model> exit_model;
    std::unique_ptr<Model> floor_model;
    std::unique_ptr<Model> arrow_model;
    std::unique_ptr<Model> arrow_trap_model;
    std::unique_ptr<Model> lava_cross_model;
    std::unique_ptr<Model> lava_horizontal_model;
    std::unique_ptr<Model> lava_vertical_model;

    GLFWwindow *window;

    /* GUI */
    friend class gui::GUI;
    gui::GUI gui;
    gui::GUIState gui_state;

    /**
     * @brief Enum that describes this client's player's lobby player state 
     * (only relevant when GUIState is set to GUIState::Lobby)
     */
    enum class LobbyPlayerState {
        Connected,
        SelectedRole,
        Ready
    };

    /**
     * @brief This client's player's lobby player state
     * (only relevant when GUIState is set to GUIState::Lobby)
     */
    LobbyPlayerState lobbyPlayerState;

    /**
     * @brief Radio button state enum for a radio button GUI
     * (represents currently selected radio button)
     */
    enum class RadioButtonState {
        NoneSelected,
        FirstOption,
        SecondOption
    };

    /**
     * @brief This client's player's lobby player role radio
     * button selection
     */
    RadioButtonState roleSelection;

    /**
     * @brief Audio Manager to play Client sounds
     */
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

    /* DM zooming in and out flags */
    bool is_held_i = false;
    bool is_held_o = false;

    bool is_left_mouse_down = false;
    bool is_right_mouse_down = false;

    /* DM Trap Orientation Traps With Directions */
    int orientation = 0;

    /* Mouse position coordinates */
    float mouse_xpos = 0.0f;
    float mouse_ypos = 0.0f;

    double lastTime = 0.0;

    GameConfig config;
    tcp::resolver resolver;
    tcp::socket socket;

    LobbyFinder lobby_finder;

    /// @brief Generate endpoints the client can connect to
    basic_resolver_results<class boost::asio::ip::tcp> endpoints;
    std::shared_ptr<Session> session;

    glm::vec3 world_pos; // stored world pause, calculated before the GUI is rendered

    std::array<boost::optional<SharedObject>, MAX_POINT_LIGHTS> closest_light_sources;

    std::deque<Event> events_received;

    /**
     * @brief boolean to see if a phase change from LOBBY to GAME already happened
     */
    bool phase_change;

    // id of last known player which is holding the orb
    EntityID player_has_orb_global_id;
};

