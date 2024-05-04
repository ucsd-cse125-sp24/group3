#include "client/core.hpp"

#include <iostream>
#include <ostream>
#include <utility>
#include <unordered_map>

#include <boost/asio/ip/tcp.hpp>
#include <boost/asio/io_service.hpp>
#include <boost/filesystem.hpp>

#include "client/cube.hpp"
#include "client/lightsource.hpp"
#include "client/shader.hpp"
#include "client/model.hpp"
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
    void handleKeys(int eid, int keyType, bool keyHeld, bool *eventSent, glm::vec3 movement = glm::vec3(0.0f));

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

    boost::filesystem::path getRootPath();

private:
    void processClientInput();
    void processServerInput(boost::asio::io_context& context);

    SharedGameState gameState;

    std::shared_ptr<Shader> cube_shader; 
    std::shared_ptr<Shader> model_shader;
    std::shared_ptr<Shader> light_source_shader;

    std::unique_ptr<Model> bear_model;
    std::unique_ptr<LightSource> light_source;

    float playerMovementDelta = 0.05f;

    GLFWwindow *window;

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

    static float mouse_xpos;
    static float mouse_ypos;

    GameConfig config;
    tcp::resolver resolver;
    tcp::socket socket;

    /// @brief Generate endpoints the client can connect to
    basic_resolver_results<class boost::asio::ip::tcp> endpoints;
    std::shared_ptr<Session> session;

    boost::filesystem::path root_path;
};

