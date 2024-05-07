#include "client/client.hpp"
#include <iostream>
#include <memory>

#include <GLFW/glfw3.h>
#include <boost/asio/ip/tcp.hpp>
#include <boost/asio/io_context.hpp>
#include <boost/archive/text_iarchive.hpp>
#include <boost/dll/runtime_symbol_info.hpp>

#include "client/lightsource.hpp"
#include "client/shader.hpp"
#include "client/model.hpp"
#include "glm/fwd.hpp"
#include "server/game/solidsurface.hpp"
#include "shared/game/event.hpp"
#include "shared/game/sharedobject.hpp"
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
bool Client::is_held_shift = false;

// Checker for events sent / later can be made in an array
glm::vec3 sentCamMovement = glm::vec3(-1.0f);

bool shiftEvent = false;

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
    gameState(GamePhase::TITLE_SCREEN, config)
{
    cam = new Camera();
    this->root_path = boost::dll::program_location().parent_path().parent_path().parent_path();
}

boost::filesystem::path Client::getRootPath() {
    return this->root_path;
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
    if (!glfwInit()) {
        const char* glfwErrorDesc = NULL;
        glfwGetError(&glfwErrorDesc);
        std::cout << "glfw init fails" << glfwErrorDesc << std::endl;
        return false;
    }

    /* Create a windowed mode window and its OpenGL context */
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    window = glfwCreateWindow(640, 480, "Arcana", NULL, NULL);
    if (!window) {
        std::cout << "could not create window" << std::endl;
        glfwTerminate();
        return false;
    }

    /* Make the window's context current */
    glfwMakeContextCurrent(window);

    // tell GLFW to capture our mouse
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    GLenum err = glewInit() ; 
    if (GLEW_OK != err) { 
        std::cerr << "Error loading GLEW: " << glewGetString(err) << std::endl; 
        return false;
    } 

    std::cout << "shader version: " << glGetString(GL_SHADING_LANGUAGE_VERSION) << std::endl;
    std::cout << "shader version: " << glGetString(GL_VERSION) << std::endl;

    boost::filesystem::path shaders_dir = this->root_path / "src/client/shaders";
    boost::filesystem::path graphics_assets_dir = this->root_path / "assets/graphics";

    boost::filesystem::path cube_vert_path = shaders_dir / "cube.vert";
    boost::filesystem::path cube_frag_path = shaders_dir / "cube.frag";
    this->cube_shader = std::make_shared<Shader>(cube_vert_path.string(), cube_frag_path.string());

    boost::filesystem::path model_vert_path = shaders_dir / "model.vert";
    boost::filesystem::path model_frag_path = shaders_dir / "model.frag";
    this->model_shader = std::make_shared<Shader>(model_vert_path.string(), model_frag_path.string());

    boost::filesystem::path bear_model_path = graphics_assets_dir / "bear-sp22.obj";
    this->bear_model = std::make_unique<Model>(bear_model_path.string());
    this->bear_model->scale(0.25);

    boost::filesystem::path player_model_path = graphics_assets_dir / "Fire-testing.obj";
    this->player_model = std::make_unique<Model>(player_model_path.string());
    this->player_model->scale(0.25);

    this->light_source = std::make_unique<LightSource>();

    boost::filesystem::path lightVertFilepath = this->root_path / "src/client/shaders/lightsource.vert";
    boost::filesystem::path lightFragFilepath = this->root_path / "src/client/shaders/lightsource.frag";
    this->light_source_shader = std::make_shared<Shader>(lightVertFilepath.string(), lightFragFilepath.string());

    return true;
}

bool Client::cleanup() {
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
    std::optional<glm::vec3> jump = glm::vec3(0.0f);
    std::optional<glm::vec3> cam_movement = glm::vec3(0.0f);

    // Sets a direction vector
    if(cam_is_held_right)
        cam_movement.value() += cam->move(false, 1.0f);
    if(cam_is_held_left)
        cam_movement.value() += cam->move(false, -1.0f);
    if (cam_is_held_up)
        cam_movement.value() += cam->move(true, 1.0f);
    if (cam_is_held_down)
        cam_movement.value() += cam->move(true, -1.0f);
    if (is_held_space)
        jump.value() += glm::vec3(0.0f, 1.0f, 0.0f);

    cam->update(mouse_xpos, mouse_ypos);

    // IF PLAYER, allow moving
    if (this->session->getInfo().client_eid.has_value()) {
        auto eid = this->session->getInfo().client_eid.value();

        this->session->sendEventAsync(Event(eid, EventType::ChangeFacing, ChangeFacingEvent(eid, cam_movement.value())));

        // Send jump action
        if (is_held_space) {
            this->session->sendEventAsync(Event(eid, EventType::StartAction, StartActionEvent(eid, jump.value(), ActionType::Jump)));
        }

        // Handles individual keys
        handleKeys(eid, GLFW_KEY_LEFT_SHIFT, is_held_shift, &shiftEvent);

        // If movement 0, send stopevent
        if ((sentCamMovement != cam_movement.value()) && cam_movement.value() == glm::vec3(0.0f)) {
            this->session->sendEventAsync(Event(eid, EventType::StopAction, StopActionEvent(eid, cam_movement.value(), ActionType::MoveCam)));
            sentCamMovement = cam_movement.value();
        }
        // If movement detected, different from previous, send start event
        else if (sentCamMovement != cam_movement.value()) {
            this->session->sendEventAsync(Event(eid, EventType::StartAction, StartActionEvent(eid, cam_movement.value(), ActionType::MoveCam)));
            sentCamMovement = cam_movement.value();
        }
    }

    processServerInput(context);
}

// Handles given key
// send startAction key is held but not sent
// send stopAction when unheld
void Client::handleKeys(int eid, int keyType, bool keyHeld, bool *eventSent, glm::vec3 movement){
    if (keyHeld == *eventSent) { return; }
    
    ActionType sendAction;
    switch(keyType) {
        case GLFW_KEY_LEFT_SHIFT:
            sendAction = ActionType::Sprint;
            break;
    }
    if (keyHeld && !*eventSent) {
        this->session->sendEventAsync(Event(eid, EventType::StartAction, StartActionEvent(eid, movement, sendAction)));
        *eventSent = true;
    }
    if (!keyHeld && *eventSent) {
        this->session->sendEventAsync(Event(eid, EventType::StopAction, StopActionEvent(eid, movement, sendAction)));
        *eventSent = false;
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

        if (sharedObject == nullptr) {
            continue;
        }

        // Get camera position from server, update position and don't render player object (or special handling)
        if (this->session->getInfo().client_eid.has_value() && sharedObject->globalID == this->session->getInfo().client_eid.value()) {
            cam->updatePos(sharedObject->physics.position);
            auto lightPos = glm::vec3(-5.0f, 0.0f, 0.0f);
            auto player_pos = glm::vec3(this->cam->getPos().x, this->cam->getPos().y - 20.0f, this->cam->getPos().z);
            this->player_model->translateAbsolute(sharedObject->physics.position);
            this->player_model->draw(
                this->model_shader,
                this->cam->getViewProj(),
                player_pos,
                lightPos,
                true);
            continue;
        }

        switch (sharedObject->type) {
            case ObjectType::Enemy: {
                // warren bear is an enemy because why not
                // auto pos = glm::vec3(0.0f, 0.0f, 0.0f);
                auto lightPos = glm::vec3(-5.0f, 0.0f, 0.0f);
                this->bear_model->translateAbsolute(sharedObject->physics.position);
                this->bear_model->draw(
                    this->model_shader,
                    this->cam->getViewProj(),
                    this->cam->getPos(),
                    lightPos,
                    true);

     /*           this->light_source->TranslateTo(lightPos);
                this->light_source->draw(
                    this->light_source_shader,
                    this->cam->getViewProj());*/

                // Cube* cube = new Cube(glm::vec3(0.4f,0.5f,0.7f));
                // cube->translateAbsolute(lightPos);
                // cube->draw(this->cube_shader,
                //     this->cam->getViewProj(),
                //     this->cam->getPos(),
                //     glm::vec3(),
                //     false);
                break;
            }
            case ObjectType::SolidSurface: {
                Cube* cube = new Cube(glm::vec3(0.4f,0.5f,0.7f));
                cube->scale( sharedObject->solidSurface->dimensions);
                cube->translateAbsolute(sharedObject->physics.position);
                cube->draw(this->cube_shader,
                    this->cam->getViewProj(),
                    this->cam->getPos(),
                    glm::vec3(),
                    true);
                break;
            }
            default:
                break;
        }
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

        case GLFW_KEY_SPACE:
            is_held_space = true;
            break;

        case GLFW_KEY_LEFT_SHIFT:
            is_held_shift = true;
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
            
        case GLFW_KEY_SPACE:
            is_held_space = false;
            break;

        case GLFW_KEY_LEFT_SHIFT:
            is_held_shift = false;
            break;

        default:
            break;
        }
    }
}

void Client::mouseCallback(GLFWwindow *window, double xposIn, double yposIn) { // cppcheck-suppress constParameterPointer
    mouse_xpos = static_cast<float>(xposIn);
    mouse_ypos = static_cast<float>(yposIn);
}
