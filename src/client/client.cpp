#include "client/client.hpp"
#include <GLFW/glfw3.h>
#include <boost/asio/ip/tcp.hpp>
#include <boost/asio/io_context.hpp>
#include <boost/archive/text_iarchive.hpp>
#include <iostream>
#include <thread>

#include "client/shaders.hpp"
#include "shared/game/event.hpp"
#include "shared/network/constants.hpp"
#include "shared/network/packet.hpp"
#include "shared/utilities/config.hpp"

using namespace boost::asio::ip;
using namespace std::chrono_literals;

// Flags
bool Client::is_held_up = false;
bool Client::is_held_down = false;
bool Client::is_held_right = false;
bool Client::is_held_left = false;
bool Client::is_held_space = false;

// For checking previous movement inputs / allows removal of repeated movement event
EventType previousVertical = EventType::Filler;
EventType previousHorizontal = EventType::Filler;

enum Direction { up, down, right, left };
Direction prevVerticalDirection = left; // set to unrelated direction
Direction prevHorizontalDirection = up; // set to unrelated direction

Client::Client(boost::asio::io_context& io_context, GameConfig config):
    resolver(io_context),
    socket(io_context),
    config(config),
    gameState(GamePhase::TITLE_SCREEN, config)
{
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
bool Client::init() {
    /* Initialize glfw library */
    if (!glfwInit())
        return false;

    /* Create a windowed mode window and its OpenGL context */
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    window = glfwCreateWindow(640, 480, "Hello World", NULL, NULL);
    if (!window) {
        glfwTerminate();
        return false;
    }

    /* Make the window's context current */
    glfwMakeContextCurrent(window);

    GLenum err = glewInit() ; 
    if (GLEW_OK != err) { 
        std::cerr << "Error loading GLEW: " << glewGetString(err) << std::endl; 
        return false;
    } 

    std::cout << "shader version: " << glGetString(GL_SHADING_LANGUAGE_VERSION) << std::endl;
    std::cout << "shader version: " << glGetString(GL_VERSION) << std::endl;

    this->cubeShaderProgram = loadCubeShaders();
    if (!this->cubeShaderProgram) {
        std::cout << "Failed to load cube shader files" << std::endl; 
        return false;
    }

    return true;
}

bool Client::cleanup() {
    glDeleteProgram(this->cubeShaderProgram);
    return true;
}

// Handles all rendering
void Client::displayCallback() {
    /* Render here */
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    if (this->gameState.phase == GamePhase::GAME) {
        this->draw();
    }

    /* Poll for and process events */
    glfwPollEvents();
    glfwSwapBuffers(window);
}

// Handle any updates 
void Client::idleCallback(boost::asio::io_context& context) {
    std::optional<glm::vec3> horizontal = glm::vec3(0.0f);
    std::optional<glm::vec3> vertical = glm::vec3(0.0f);
    std::optional<glm::vec3> jump = glm::vec3(0.0f);

    // Sets a direction vector
    if(is_held_right)
        horizontal.value() += glm::vec3(1.0f, 0.0f, 0.0f);
    if(is_held_left)
        horizontal.value() += glm::vec3(-1.0f, 0.0f, 0.0f);
    if (is_held_up)
        vertical.value() += glm::vec3(0.0f, 0.0f, -1.0f);
    if (is_held_down)
        vertical.value() += glm::vec3(0.0f, 0.0f, 1.0f);
    if (is_held_space)
        jump.value() += glm::vec3(0.0f, 1.0f, 0.0f);

    while (true) {
        if (is_held_space) {
            auto eid = 0;
            this->session->sendEventAsync(Event(eid, EventType::Jump, JumpEvent(eid, jump.value())));
            break;
        }
        else {
            break;
        }
    }

    // Checks for right/left key inputs
    while (true) {
        // don't send any event if same key is pressed
        if (previousHorizontal == EventType::HorizontalKeyDown && ((prevHorizontalDirection == right && is_held_right) || (prevHorizontalDirection == left && is_held_left))) { break; }

        // sends event if key is pressed
        if (is_held_right) {
            auto eid = 0; // TODO: later set eid to player id or etc.
            this->session->sendEventAsync(Event(eid, EventType::HorizontalKeyDown, HorizontalKeyDownEvent(eid, horizontal.value())));
            previousHorizontal = EventType::HorizontalKeyDown;
            prevHorizontalDirection = right;

        }
        else if (is_held_left) {
            auto eid = 0;
            this->session->sendEventAsync(Event(eid, EventType::HorizontalKeyDown, HorizontalKeyDownEvent(eid, horizontal.value())));
            previousHorizontal = EventType::HorizontalKeyDown;
            prevHorizontalDirection = left;
            break;
        }
        else { // if both left/right key is up, stop horizontal movement
            if (previousHorizontal == EventType::HorizontalKeyUp) { break; }
            auto eid = 0;
            this->session->sendEventAsync(Event(eid, EventType::HorizontalKeyUp, HorizontalKeyUpEvent(eid, horizontal.value())));
            previousHorizontal = EventType::HorizontalKeyUp;
            prevHorizontalDirection = up; // reset prev direction to unrelated direction
            break;
        }
    }

    // Checks for up/down key inputs
    while (true) {
        // don't send any event if same key is pressed
        if (previousVertical == EventType::VerticalKeyDown && ((prevVerticalDirection == up && is_held_up) || (prevVerticalDirection == down && is_held_down))) { break; }

        // sends event if key is pressed
        if (is_held_up) {
            auto eid = 0;
            this->session->sendEventAsync(Event(eid, EventType::VerticalKeyDown, VerticalKeyDownEvent(eid, vertical.value())));
            previousVertical = EventType::VerticalKeyDown;
            prevVerticalDirection = up;
            break;
        }
        else if (is_held_down) {
            auto eid = 0;
            this->session->sendEventAsync(Event(eid, EventType::VerticalKeyDown, VerticalKeyDownEvent(eid, vertical.value())));
            previousVertical = EventType::VerticalKeyDown;
            prevVerticalDirection = down;
            break;
        }
        else { // if both up/down key is up, stop vertical movement
            if (previousVertical == EventType::VerticalKeyUp) { break; }
            auto eid = 0;
            this->session->sendEventAsync(Event(eid, EventType::VerticalKeyUp, VerticalKeyUpEvent(eid, vertical.value())));
            previousVertical = EventType::VerticalKeyUp;
            prevVerticalDirection = left; // reset prev direction to unrelated direction
            break;
        }
    }

    processServerInput(context);
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

        std::cout << "got an object" << std::endl;
        //  tmp: all objects are cubes
        Cube* cube = new Cube();
        cube->update(sharedObject->physics.position);
        cube->draw(this->cubeShaderProgram);
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

        case GLFW_KEY_SPACE:
            is_held_space = true;
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

        case GLFW_KEY_SPACE:
            is_held_space = false;
            break;

        default:
            break;
        }
    }
}

