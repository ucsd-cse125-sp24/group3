#include "client/client.hpp"
#include <GLFW/glfw3.h>
#include <boost/asio/ip/tcp.hpp>
#include <boost/asio/io_context.hpp>
#include <boost/archive/text_iarchive.hpp>
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

#include <iostream>
#include <thread>

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

Client::Client(boost::asio::io_context& io_context, GameConfig config):
    resolver(io_context),
    socket(io_context),
    config(config),
    gameState(GamePhase::TITLE_SCREEN, config),
    session(nullptr)
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

    GLenum err = glewInit() ; 
    if (GLEW_OK != err) { 
        std::cerr << "Error: " << glewGetString(err) << std::endl; 
    } 

    std::cout << "shader version: " << glGetString(GL_SHADING_LANGUAGE_VERSION) << std::endl;
    std::cout << "shader version: " << glGetString(GL_VERSION) << std::endl;

    /* Load shader programs */
    std::cout << "loading shader" << std::endl;
    shaderProgram = LoadShaders("../src/client/shaders/shader.vert", "../src/client/shaders/shader.frag");

    // Check the shader program.
    if (!shaderProgram) {
        std::cerr << "Failed to initialize shader program" << std::endl;
        return false;
    }

    // Set up IMGUI
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& imGuiIO = ImGui::GetIO();
    imGuiIO.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
    imGuiIO.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls
    // imGuiIO.ConfigFlags |= ImGuiConfigFlags_DockingEnable;         // IF using Docking Branch

    // Setup Platform/Renderer backends
    ImGui_ImplGlfw_InitForOpenGL(window, true);          // Second param install_callback=true will install GLFW callbacks and chain to existing ones.
    ImGui_ImplOpenGL3_Init();

    return 0;
}

int Client::cleanup() {
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    glDeleteProgram(shaderProgram);
    return 0;
}

// Handles all rendering
void Client::displayCallback() {
    /* Render here */
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

    if (this->gameState.phase == GamePhase::TITLE_SCREEN) {
        
    } else if (this->gameState.phase == GamePhase::GAME) {
        this->draw();
    }

    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

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

        std::cout << "got an object" << std::endl;
        //  tmp: all objects are cubes
        Cube* cube = new Cube();
        cube->update(sharedObject->physics.position);
        cube->draw(this->shaderProgram);
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

        default:
            break;
        }
    }
}

