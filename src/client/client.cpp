#include "client/client.hpp"
#include <GLFW/glfw3.h>
#include <boost/asio/ip/tcp.hpp>
#include <boost/asio/io_context.hpp>
#include <boost/archive/text_iarchive.hpp>
// #include "imgui.h"
// #include "imgui_impl_glfw.h"
// #include "imgui_impl_opengl3.h"

#include <iostream>
#include <thread>

#include "shared/game/event.hpp"
#include "shared/network/constants.hpp"
#include "shared/network/packet.hpp"
#include "shared/utilities/config.hpp"
#include "shared/utilities/root_path.hpp"

using namespace boost::asio::ip;
using namespace std::chrono_literals;

// Flags
bool Client::is_held_up = false;
bool Client::is_held_down = false;
bool Client::is_held_right = false;
bool Client::is_held_left = false;

bool Client::cam_is_held_up = false;
bool Client::cam_is_held_down = false;
bool Client::cam_is_held_right = false;
bool Client::cam_is_held_left = false;

float Client::mouse_xpos = 0.0f;
float Client::mouse_ypos = 0.0f;

Client::Client(boost::asio::io_context& io_context, GameConfig config):
    resolver(io_context),
    socket(io_context),
    config(config),
    gameState(GamePhase::TITLE_SCREEN, config),
    session(nullptr),
    gui()
{
    cam = new Camera();
}

void Client::connectAndListen(std::string ip_addr) {
    this->endpoints = resolver.resolve(ip_addr, std::to_string(config.network.server_port));
    this->session = std::make_shared<Session>(std::move(this->socket),
        SessionInfo(this->config.client.default_name, {}));

    this->session->connectTo(this->endpoints);

    auto packet = PackagedPacket::make_shared(PacketType::ClientDeclareInfo,
        ClientDeclareInfoPacket { .player_name = config.client.default_name });

    this->session->sendPacketAsync(packet);

    this->session->startListen();
}

Client::~Client() {

}

// TODO: error flags / output for broken init
int Client::init() {
    /* Initialize glfw library */
    if (!glfwInit())
        return -1;

    /* Create a windowed mode window and its OpenGL context */
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    window = glfwCreateWindow(640, 480, "Hello World", NULL, NULL);
    if (!window)
    {
        glfwTerminate();
        return -1;
    }

    /* Make the window's context current */
    glfwMakeContextCurrent(window);

    // tell GLFW to capture our mouse
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    GLenum err = glewInit() ; 
    if (GLEW_OK != err) { 
        std::cerr << "Error: " << glewGetString(err) << std::endl; 
    } 

    std::cout << "shader version: " << glGetString(GL_SHADING_LANGUAGE_VERSION) << std::endl;
    std::cout << "shader version: " << glGetString(GL_VERSION) << std::endl;

    /* Load shader programs */
    std::cout << "loading shader" << std::endl;
    auto shader_path = getRepoRoot() / "src" / "client" / "shaders";
    shaderProgram = LoadShaders((shader_path / "shader.vert").c_str(), (shader_path / "shader.frag").c_str());
    auto textShaderProgram = LoadShaders((shader_path / "text.vert").c_str(), (shader_path / "text.frag").c_str());

    if (!shaderProgram) {
        std::cerr << "Failed to initialize shader program" << std::endl;
        return -1;
    }

    if (!textShaderProgram) {
        std::cerr << "Failed to initialize text shader program" << std::endl;
        return -1;
    }

    // Init GUI (e.g. load in all fonts)
    if (!this->gui.init(textShaderProgram)) {
        std::cerr << "GUI failed to init" << std::endl;
        return -1;
    }

    return 0;
}

int Client::cleanup() {
    glDeleteProgram(shaderProgram);
    return 0;
}

// Handles all rendering
void Client::displayCallback() {
    /* Render here */
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    this->gui.render();

    if (this->gameState.phase == GamePhase::TITLE_SCREEN) {
        
    } else if (this->gameState.phase == GamePhase::GAME) {
        this->draw();
    }

    /* Poll for and process events */
    glfwPollEvents();
    glfwSwapBuffers(window);
}

// Handle any updates 
void Client::idleCallback(boost::asio::io_context& context) {
    std::optional<glm::vec3> movement = glm::vec3(0.0f);

    if(is_held_right)
        movement.value() += glm::vec3(cubeMovementDelta, 0.0f, 0.0f);
    if(is_held_left)
        movement.value() += glm::vec3(-cubeMovementDelta, 0.0f, 0.0f);
    if(is_held_up)
        movement.value() += glm::vec3(0.0f, cubeMovementDelta, 0.0f);
    if(is_held_down)
        movement.value() += glm::vec3(0.0f, -cubeMovementDelta, 0.0f);

    std::optional<glm::vec3> cam_movement = glm::vec3(0.0f);
    if(cam_is_held_right)
        cam_movement.value() += cam->move(false, 1.0f);
    if(cam_is_held_left)
        cam_movement.value() += cam->move(false, -1.0f);
    if(cam_is_held_up)
        cam_movement.value() += cam->move(true, 1.0f);
    if(cam_is_held_down)
        cam_movement.value() += cam->move(true, -1.0f);

    cam->update(mouse_xpos, mouse_ypos);

    if (movement.has_value() && this->session != nullptr) {
        auto eid = 0; 
        this->session->sendEventAsync(Event(eid, EventType::MoveRelative, MoveRelativeEvent(eid, movement.value())));
    }

    if (this->session != nullptr) {
        processServerInput(context);
    }
}

void Client::processServerInput(boost::asio::io_context& context) {
    context.run_for(30ms);

    // probably want to put rendering logic inside of client, so that this main function
    // mimics the server one where all of the important logic is done inside of a run command
    // But this is a demo of how you could use the client session to get information from
    // the game state

    for (Event event : this->session->getEvents()) {
        if (event.type == EventType::LoadGameState) {
            this->gameState = boost::get<LoadGameStateEvent>(event.data).state;
        }
    }
}

void Client::draw() {
    for (int i = 0; i < this->gameState.objects.size(); i++) {
        std::shared_ptr<SharedObject> sharedObject = this->gameState.objects.at(i);

        if (sharedObject == nullptr)
            continue;

        // std::cout << "got an object" << std::endl;
        //  tmp: all objects are cubes
        Cube* cube = new Cube();
        cube->update(sharedObject->physics.position);
        
        cube->draw(this->cam->getViewProj(), this->shaderProgram);
    }
}

// callbacks - for Interaction
void Client::keyCallback(GLFWwindow *window, int key, int scancode, int action, int mods) {
  // Check for a key press.
    if (action == GLFW_PRESS) {
        switch (key) {
        case GLFW_KEY_ESCAPE:
            // Close the window. This causes the program to also terminate.
            glfwSetWindowShouldClose(window, GL_TRUE);
            break;

        case GLFW_KEY_DOWN:
            is_held_down = true;
            break;

        case GLFW_KEY_UP:
            is_held_up = true;
            break;

        case GLFW_KEY_LEFT:
            is_held_left = true;
            break;

        case GLFW_KEY_RIGHT:
            is_held_right = true;
            break;

        case GLFW_KEY_S:
            cam_is_held_down = true;
            break;

        case GLFW_KEY_W:
            cam_is_held_up = true;
            break;

        case GLFW_KEY_A:
            cam_is_held_left = true;
            break;

        case GLFW_KEY_D:
            cam_is_held_right = true;
            break;

        default:
            break;
        }
    }

    if (action == GLFW_RELEASE) {
        switch (key) {
        case GLFW_KEY_DOWN:
            is_held_down = false;
            break;

        case GLFW_KEY_UP:
            is_held_up = false;
            break;

        case GLFW_KEY_LEFT:
            is_held_left = false;
            break;

        case GLFW_KEY_RIGHT:
            is_held_right = false;
            break;

        case GLFW_KEY_S:
            cam_is_held_down = false;
            break;

        case GLFW_KEY_W:
            cam_is_held_up = false;
            break;

        case GLFW_KEY_A:
            cam_is_held_left = false;
            break;

        case GLFW_KEY_D:
            cam_is_held_right = false;
            break;
            
        default:
            break;
        }
    }
}

void Client::mouseCallback(GLFWwindow* window, double xposIn, double yposIn) {
    mouse_xpos = static_cast<float>(xposIn);
    mouse_ypos = static_cast<float>(yposIn);
}
