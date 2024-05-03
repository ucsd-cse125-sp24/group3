#include "client/client.hpp"

#include <iostream>
#include <memory>

#include <GLFW/glfw3.h>
#include <boost/asio/ip/tcp.hpp>
#include <boost/asio/io_context.hpp>
#include <boost/archive/text_iarchive.hpp>

#include <iostream>
#include <thread>
#include <sstream>

#include "client/gui/gui.hpp"
#include <boost/dll/runtime_symbol_info.hpp>

#include "client/shader.hpp"
#include "shared/game/event.hpp"
#include "shared/network/constants.hpp"
#include "shared/network/packet.hpp"
#include "shared/utilities/config.hpp"
#include "shared/utilities/root_path.hpp"
#include "shared/utilities/time.hpp"

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
bool Client::is_left_mouse_down = false;

float Client::mouse_xpos = 0.0f;
float Client::mouse_ypos = 0.0f;

time_t Client::time_of_last_keystroke = 0;

using namespace gui;

Client::Client(boost::asio::io_context& io_context, GameConfig config):
    resolver(io_context),
    socket(io_context),
    config(config),
    gameState(GamePhase::TITLE_SCREEN, config),
    session(nullptr),
    gui(this),
    gui_state(gui::GUIState::TITLE_SCREEN),
    lobby_finder(io_context, config)
{
    cam = new Camera();
    
    if (config.client.lobby_discovery)  {
        lobby_finder.startSearching();
    }
}

void Client::connectAndListen(std::string ip_addr) {
    this->endpoints = resolver.resolve(ip_addr, std::to_string(config.network.server_port));
    this->session = std::make_shared<Session>(std::move(this->socket),
        SessionInfo(this->config.client.default_name, {}));

    this->session->connectTo(this->endpoints);

    auto name = this->gui.getCapturedKeyboardInput();
    if (name == "") {
        name = config.client.default_name;
    } 

    auto packet = PackagedPacket::make_shared(PacketType::ClientDeclareInfo,
        ClientDeclareInfoPacket { .player_name = name });

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
    window = glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "Arcana", NULL, NULL);
    if (!window)
    {
        glfwTerminate();
        return false;
    }
    glfwSetWindowSizeLimits(window, WINDOW_WIDTH, WINDOW_HEIGHT, WINDOW_WIDTH, WINDOW_HEIGHT);

    /* Make the window's context current */
    glfwMakeContextCurrent(window);


    GLenum err = glewInit() ; 
    if (GLEW_OK != err) { 
        std::cerr << "Error loading GLEW: " << glewGetString(err) << std::endl; 
        return false;
    } 

    std::cout << "shader version: " << glGetString(GL_SHADING_LANGUAGE_VERSION) << std::endl;
    std::cout << "shader version: " << glGetString(GL_VERSION) << std::endl;

    /* Load shader programs */
    std::cout << "loading shader" << std::endl;
    auto shader_path = getRepoRoot() / "src" / "client" / "shaders";
    auto textShaderProgram = LoadShaders((shader_path / "text.vert").c_str(), (shader_path / "text.frag").c_str());

    if (!textShaderProgram) {
        std::cerr << "Failed to initialize text shader program" << std::endl;
        return false;
    }

    // Init GUI (e.g. load in all fonts)
    // TODO: pass in shader for image loading in second param
    if (!this->gui.init(textShaderProgram, textShaderProgram)) {
        std::cerr << "GUI failed to init" << std::endl;
        return false;
    }

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

    this->gui.beginFrame();

    if (this->gameState.phase == GamePhase::TITLE_SCREEN) {
        
    } else if (this->gameState.phase == GamePhase::LOBBY) {
    } else if (this->gameState.phase == GamePhase::GAME) {
        this->draw();
    }

    this->gui.layoutFrame(this->gui_state);
    this->gui.handleInputs(mouse_xpos, mouse_ypos, is_left_mouse_down);
    this->gui.renderFrame();

    /* Poll for and process events */
    glfwPollEvents();
    glfwSwapBuffers(window);
}

// Handle any updates 
void Client::idleCallback(boost::asio::io_context& context) {
    if (this->session != nullptr) {
        processServerInput(context);
    }

    // If we aren't in the middle of the game then we shouldn't capture any movement info
    // or send any movement related events
    if (this->gui_state != GUIState::GAME_HUD) { return; }

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
    if (this->session != nullptr && this->session->getInfo().client_eid.has_value()) {
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
            GamePhase old_phase = this->gameState.phase;
            this->gameState = boost::get<LoadGameStateEvent>(event.data).state;

            // Change the UI to the game hud UI whenever we change into the GAME game phase
            if (old_phase != GamePhase::GAME && this->gameState.phase == GamePhase::GAME) {
                this->gui_state = GUIState::GAME_HUD;
            }
        }
    }
}

void Client::draw() {
    for (int i = 0; i < this->gameState.objects.size(); i++) {
        std::shared_ptr<SharedObject> sharedObject = this->gameState.objects.at(i);

        if (sharedObject == nullptr)
            continue;

        // Get camera position from server, update position and don't render player object (or special handling)
        if (this->session->getInfo().client_eid.has_value() && sharedObject->globalID == this->session->getInfo().client_eid.value()) {
            cam->updatePos(sharedObject->physics.position);
            continue;
        }

        // If solidsurface, scale cube to given dimensions
        if(sharedObject->solidSurface.has_value()){
            Cube* cube = new Cube(glm::vec3(0.4f,0.5f,0.7f), sharedObject->solidSurface->dimensions);
            cube->update(sharedObject->physics.position);
            cube->draw(this->cam->getViewProj(), this->cubeShaderProgram, true);
            continue;
        }

        //  tmp: all objects are cubes
        Cube* cube = new Cube(glm::vec3(0.0f,1.0f,1.0f), glm::vec3(1.0f));
        cube->update(sharedObject->physics.position);
        cube->draw(this->cam->getViewProj(), this->cubeShaderProgram, false);
    }
}

// callbacks - for Interaction
void Client::keyCallback(GLFWwindow *window, int key, int scancode, int action, int mods) {
    Client* client = static_cast<Client*>(glfwGetWindowUserPointer(window));

    // Check for a key press.
    if (action == GLFW_PRESS) {
        switch (key) {
        case GLFW_KEY_ESCAPE:
            if (client->gameState.phase == GamePhase::GAME) {
                if (client->gui_state == GUIState::GAME_ESC_MENU) {
                    client->gui_state = GUIState::GAME_HUD;
                } else if (client->gui_state == GUIState::GAME_HUD) {
                    client->gui_state = GUIState::GAME_ESC_MENU;
                }
            }
            client->gui.setCaptureKeystrokes(false);
            break;
        
        case GLFW_KEY_TAB:
            client->gui.setCaptureKeystrokes(true);
            break;
        
        case GLFW_KEY_BACKSPACE:
            client->gui.captureBackspace();
            Client::time_of_last_keystroke = getMsSinceEpoch();
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

    if (action == GLFW_REPEAT) {
        if (key == GLFW_KEY_BACKSPACE) {
            auto ms_since_epoch = getMsSinceEpoch();
            if (Client::time_of_last_keystroke + 100 < ms_since_epoch) {
                Client::time_of_last_keystroke = ms_since_epoch;
                client->gui.captureBackspace();
            }
        }
    }
}

void Client::mouseCallback(GLFWwindow *window, double xposIn, double yposIn) { // cppcheck-suppress constParameterPointer
    mouse_xpos = static_cast<float>(xposIn);
    mouse_ypos = static_cast<float>(yposIn);
}

void Client::mouseButtonCallback(GLFWwindow* window, int button, int action, int mods) {
    if (button == GLFW_MOUSE_BUTTON_LEFT) {
        if (action == GLFW_PRESS) {
            is_left_mouse_down = true;
        } else if (action == GLFW_RELEASE) {
            is_left_mouse_down = false;
        }
    }
}

void Client::charCallback(GLFWwindow* window, unsigned int codepoint) {
    Client* client = static_cast<Client*>(glfwGetWindowUserPointer(window));

    client->gui.captureKeystroke(static_cast<char>(codepoint));
    Client::time_of_last_keystroke = getMsSinceEpoch();
}

time_t Client::getTimeOfLastKeystroke() {
    return Client::time_of_last_keystroke;
}