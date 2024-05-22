#include "client/client.hpp"
#include <iostream>
#include <algorithm>
#include <functional>
#include <iterator>
#include <memory>

#include <GLFW/glfw3.h>
#include <boost/asio/ip/tcp.hpp>
#include <boost/asio/io_context.hpp>
#include <boost/archive/text_iarchive.hpp>

#include <iostream>
#include <thread>
#include <sstream>

#include "client/util.hpp"
#include "client/gui/gui.hpp"
#include "client/constants.hpp"
#include <boost/dll/runtime_symbol_info.hpp>

#include "client/lightsource.hpp"
#include "client/shader.hpp"
#include "client/model.hpp"
#include "glm/fwd.hpp"
#include "server/game/object.hpp"
#include "server/game/solidsurface.hpp"
#include "shared/game/event.hpp"
#include "shared/game/sharedobject.hpp"
#include "shared/network/constants.hpp"
#include "shared/network/packet.hpp"
#include "shared/utilities/config.hpp"
#include "client/audiomanager.hpp"
#include "shared/utilities/root_path.hpp"
#include "shared/utilities/time.hpp"
#include "shared/game/celltype.hpp"
#include "shared/utilities/timer.hpp"
#include "shared/utilities/typedefs.hpp"

#define GLM_ENABLE_EXPERIMENTAL
#include "glm/ext/matrix_clip_space.hpp"
#include "glm/fwd.hpp"
#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtx/string_cast.hpp>
#include <glm/gtx/euler_angles.hpp>


using namespace boost::asio::ip;
using namespace std::chrono_literals;

// Checker for events sent / later can be made in an array
glm::vec3 sentCamMovement = glm::vec3(-1.0f);

int Client::window_width = UNIT_WINDOW_WIDTH;
int Client::window_height = UNIT_WINDOW_HEIGHT;

time_t Client::time_of_last_keystroke = 0;

using namespace gui;

Client::Client(boost::asio::io_context& io_context, GameConfig config):
    resolver(io_context),
    socket(io_context),
    config(config),
    gameState(GamePhase::TITLE_SCREEN, config),
    session(nullptr),
    gui(this),
    gui_state(gui::GUIState::INITIAL_LOAD),
    lobby_finder(io_context, config),
    cam(new Camera()) {    
    
    audioManager = new AudioManager();

    Client::window_width = config.client.window_width;
    Client::window_height = static_cast<int>((config.client.window_width * 2.0f) / 3.0f);

    if (config.client.lobby_discovery)  {
        lobby_finder.startSearching();
    }
}

AudioManager* Client::getAudioManager() {
    return this->audioManager;
}

bool Client::connectAndListen(std::string ip_addr) {
    this->endpoints = resolver.resolve(ip_addr, std::to_string(config.network.server_port));
    this->session = std::make_shared<Session>(std::move(this->socket),
        SessionInfo(this->config.client.default_name, {}, {}));

    if (!this->session->connectTo(this->endpoints)) {
        return false;
    }

    auto name = this->gui.getCapturedKeyboardInput();
    if (name == "") {
        name = config.client.default_name;
    } 

    auto packet = PackagedPacket::make_shared(PacketType::ClientDeclareInfo,
        ClientDeclareInfoPacket { .player_name = name });

    this->session->sendPacketAsync(packet);

    this->session->startListen();
    return true;
}

Client::~Client() {}

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
    window = glfwCreateWindow(Client::window_width, Client::window_height, "Arcana", NULL, NULL);
    if (!window)
    {
        glfwTerminate();
        return false;
    }
    glfwSetWindowSizeLimits(window, 
        Client::window_width, Client::window_height, Client::window_width, Client::window_height);

    /* Make the window's context current */
    glfwMakeContextCurrent(window);

    GLenum err = glewInit() ; 
    if (GLEW_OK != err) { 
        std::cerr << "Error loading GLEW: " << glewGetString(err) << std::endl; 
        return false;
    }

    std::cout << "shader version: " << glGetString(GL_SHADING_LANGUAGE_VERSION) << std::endl;
    std::cout << "shader version: " << glGetString(GL_VERSION) << std::endl;

    // Init GUI (e.g. load in all fonts)
    // TODO: pass in shader for image loading in second param
    if (!this->gui.init()) {
        std::cerr << "GUI failed to init" << std::endl;
        return false;
    }

    this->displayCallback();

    auto shaders_dir = getRepoRoot() / "src/client/shaders";
    auto graphics_assets_dir = getRepoRoot() / "assets/graphics";

    auto cube_vert_path = shaders_dir / "cube.vert";
    auto cube_frag_path = shaders_dir / "cube.frag";
    this->cube_shader = std::make_shared<Shader>(cube_vert_path.string(), cube_frag_path.string());    
    
    auto dm_cube_frag_path = shaders_dir / "dm_cube.frag";
    this->dm_cube_shader = std::make_shared<Shader>(cube_vert_path.string(), dm_cube_frag_path.string());

    auto model_vert_path = shaders_dir / "model.vert";
    auto model_frag_path = shaders_dir / "model.frag";
    this->model_shader = std::make_shared<Shader>(model_vert_path.string(), model_frag_path.string());

    auto cube_model_path = graphics_assets_dir / "cube.obj";
    this->cube_model = std::make_unique<Model>(cube_model_path.string());

    auto bear_model_path = graphics_assets_dir / "bear-sp22.obj";
    this->bear_model = std::make_unique<Model>(bear_model_path.string());
    // this->bear_model->scaleAbsolute(0.25);

    auto player_model_path = graphics_assets_dir / "Fire-testing.obj";
    this->player_model = std::make_unique<Model>(player_model_path.string());
    this->player_model->scaleAbsolute(0.25);

    this->light_source = std::make_unique<LightSource>();

    auto lightVertFilepath = shaders_dir / "lightsource.vert";
    auto lightFragFilepath = shaders_dir / "lightsource.frag";
    this->light_source_shader = std::make_shared<Shader>(lightVertFilepath.string(), lightFragFilepath.string());

    auto torchlight_model_path = graphics_assets_dir / "cube.obj";
    this->torchlight_model = std::make_unique<Model>(torchlight_model_path.string());

    auto solid_surface_vert_path = shaders_dir / "solidsurface.vert";
    auto solid_surface_frag_path = shaders_dir / "solidsurface.frag";
    this->solid_surface_shader = std::make_shared<Shader>(solid_surface_vert_path.string(), solid_surface_frag_path.string());

    this->gui_state = GUIState::TITLE_SCREEN;

    this->audioManager->init();

    return true;
}

bool Client::cleanup() {
    cam.reset(nullptr);

    // Destroy the window.
    glfwDestroyWindow(window);
    // Terminate GLFW.
    glfwTerminate();

    delete audioManager;
    return true;
}

// Handles all rendering
void Client::displayCallback() {
    /* Render here */
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    this->gui.beginFrame();

    if (this->gameState.phase == GamePhase::TITLE_SCREEN) {
        glClearColor(0.8f, 0.8f, 0.8f, 1.0f);
    } else if (this->gameState.phase == GamePhase::LOBBY) {
        glClearColor(0.8f, 0.8f, 0.8f, 1.0f);
    } else if (this->gameState.phase == GamePhase::GAME) {
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        this->draw();
    }
    else if (this->gameState.phase == GamePhase::RESULTS) {
        if (this->gui_state == GUIState::GAME_HUD)
            this->gui_state = GUIState::RESULTS_SCREEN;
        this->draw();
    }

    this->setWorldPos();

    this->gui.layoutFrame(this->gui_state);
    this->gui.handleInputs(mouse_xpos, mouse_ypos, is_left_mouse_down);
    this->gui.renderFrame();

    /* Poll for and process events */
    glfwPollEvents();
    glfwSwapBuffers(window);

    //  DEBUG
    //std::cout << "playerVictory: " << this->gameState.playerVictory << std::endl;
    //  DEBUG
}

// Handle any updates 
void Client::idleCallback(boost::asio::io_context& context) {
    if (this->session != nullptr) {
        processServerInput(context);
    }

    // If we aren't in the middle of the game then we shouldn't capture any movement info
    // or send any movement related events
    if (this->gui_state != GUIState::GAME_HUD) { return; }

    glm::vec3 cam_movement = glm::vec3(0.0f);

    // Sets a direction vector
    if(is_held_right)
        cam_movement += cam->move(true, 1.0f);
    if(is_held_left)
        cam_movement += cam->move(true, -1.0f);
    if (is_held_up)
        cam_movement += cam->move(false, 1.0f);
    if (is_held_down)
        cam_movement += cam->move(false, -1.0f);

    // Update camera facing direction
    cam->update(mouse_xpos, mouse_ypos);

    // IF PLAYER, allow moving
    if (this->session != nullptr && this->session->getInfo().client_eid.has_value()) {
        auto eid = this->session->getInfo().client_eid.value();

        this->session->sendEventAsync(Event(eid, EventType::ChangeFacing, ChangeFacingEvent(eid, cam->getFacing())));

        // Send jump action
        if (is_held_space) {
            this->session->sendEventAsync(Event(eid, EventType::StartAction, StartActionEvent(eid, glm::vec3(0.0f, 1.0f, 0.0f), ActionType::Jump)));
        }

        if (this->session->getInfo().is_dungeon_master.value() && is_held_i) {
            this->session->sendEventAsync(Event(eid, EventType::StartAction, StartActionEvent(eid, glm::vec3(0.0f, -1.0f, 0.0f), ActionType::Zoom)));
        }

        if (this->session->getInfo().is_dungeon_master.value() && is_held_o) {
            this->session->sendEventAsync(Event(eid, EventType::StartAction, StartActionEvent(eid, glm::vec3(0.0f, 1.0f, 0.0f), ActionType::Zoom)));
        }

        if (this->session->getInfo().is_dungeon_master.value() && is_pressed_p && is_left_mouse_down) {
            auto self = this->gameState.objects.at(eid);

            auto selectedTrap = self->trapInventoryInfo->inventory[self->trapInventoryInfo->selected - 1];

            switch (selectedTrap) {
            case ModelType::FloorSpikeFull:
                std::cout << "Sending full\n" << std::endl;

                this->session->sendEventAsync(Event(eid, EventType::TrapPlacement, TrapPlacementEvent(eid, this->world_pos, CellType::FloorSpikeFull, false, true)));
                break;
            case ModelType::FloorSpikeVertical:
                std::cout << "Sending vertical\n" << std::endl;

                this->session->sendEventAsync(Event(eid, EventType::TrapPlacement, TrapPlacementEvent(eid, this->world_pos, CellType::FloorSpikeVertical, false, true)));
                break;
            case ModelType::FloorSpikeHorizontal:
                std::cout << "Sending horizontal\n" << std::endl;
                this->session->sendEventAsync(Event(eid, EventType::TrapPlacement, TrapPlacementEvent(eid, this->world_pos, CellType::FloorSpikeHorizontal, false, true)));
                break;
            case ModelType::FireballTrap:
                this->session->sendEventAsync(Event(eid, EventType::TrapPlacement, TrapPlacementEvent(eid, this->world_pos, CellType::FireballTrap, false, true)));
                break;
            case ModelType::SpikeTrap:
                this->session->sendEventAsync(Event(eid, EventType::TrapPlacement, TrapPlacementEvent(eid, this->world_pos, CellType::SpikeTrap, false, true)));
                break;
            }
        }

        // If movement 0, send stopevent
        if ((sentCamMovement != cam_movement) && cam_movement == glm::vec3(0.0f)) {
            this->session->sendEventAsync(Event(eid, EventType::StopAction, StopActionEvent(eid, cam_movement, ActionType::MoveCam)));
            sentCamMovement = cam_movement;
        }

        // If movement detected, different from previous, send start event
        else if (sentCamMovement != cam_movement) {
            this->session->sendEventAsync(Event(eid, EventType::StartAction, StartActionEvent(eid, cam_movement, ActionType::MoveCam)));
            sentCamMovement = cam_movement;
        }
    }
}

void Client::processServerInput(boost::asio::io_context& context) {
    context.run_for(5ms);

    // probably want to put rendering logic inside of client, so that this main function
    // mimics the server one where all of the important logic is done inside of a run command
    // But this is a demo of how you could use the client session to get information from
    // the game state

    for (Event event : this->session->getEvents()) {
        if (event.type == EventType::LoadGameState) {
            GamePhase old_phase = this->gameState.phase;
            this->gameState.update(boost::get<LoadGameStateEvent>(event.data).state);

            // Change the UI to the game hud UI whenever we change into the GAME game phase
            if (old_phase != GamePhase::GAME && this->gameState.phase == GamePhase::GAME) {
                // set to Dungeon Master POV if DM
                if (this->session->getInfo().is_dungeon_master) {
                    std::cout << "DUNGEON MASTER CAM!" << std::endl;
                    this->cam = std::make_unique<DungeonMasterCamera>();
                }

                this->gui_state = GUIState::GAME_HUD;
            }
        } else if (event.type == EventType::UpdateLightSources) {
            const auto& updated_light_source = boost::get<UpdateLightSourcesEvent>(event.data);
            for (int i = 0; i < closest_light_sources.size(); i++) {

                if (!updated_light_source.lightSources[i].has_value()) {
                    continue;
                }
                EntityID light_id = updated_light_source.lightSources[i].value().eid;
                this->closest_light_sources[i] = this->gameState.objects[light_id];
                // update intensity with incoming intensity 
                this->closest_light_sources[i]->pointLightInfo->intensity = updated_light_source.lightSources[i]->intensity;
            }
        }
    }
}

void Client::draw() {
    if (!this->session->getInfo().client_eid.has_value()) {
        return;
    }
    auto eid = this->session->getInfo().client_eid.value();
    glm::vec3 my_pos = this->gameState.objects[eid]->physics.corner;
    for (auto& [id, sharedObject] : this->gameState.objects) {

        if (!sharedObject.has_value()) {
            continue;
        }
        auto dist = glm::distance(sharedObject->physics.corner, my_pos);

        if (!this->session->getInfo().is_dungeon_master.value() && dist > RENDER_DISTANCE) {
            continue;
        }

        switch (sharedObject->type) {
            case ObjectType::Player: {
                // don't render yourself
                if (this->session->getInfo().client_eid.has_value() && sharedObject->globalID == this->session->getInfo().client_eid.value()) {
                    //  TODO: Update the player eye level to an acceptable level
                    glm::vec3 pos = sharedObject->physics.getCenterPosition();
                    pos.y += PLAYER_EYE_LEVEL;
                    cam->updatePos(pos);

                    // reset back to game mode if this is the first frame in which you are respawned
                    if (this->gui_state == GUIState::DEAD_SCREEN && sharedObject->playerInfo->is_alive) {
                        this->gui_state = GUIState::GAME_HUD;
                    }

                    // Check if you are actually still alive
                    if (!sharedObject->playerInfo->is_alive) {
                        this->gui_state = GUIState::DEAD_SCREEN;
                    }
                    break;
                }

                if (!sharedObject->playerInfo->render) { break; } // dont render while invisible
                auto lightPos = glm::vec3(0.0f, 10.0f, 0.0f);

                auto player_pos = sharedObject->physics.getCenterPosition();

                this->player_model->translateAbsolute(player_pos);
                this->player_model->draw(
                    this->model_shader,
                    this->cam->getViewProj(),
                    this->cam->getPos(),
                    {},
                    true);
                break;
            }
            // CHANGE THIS
            case ObjectType::DungeonMaster: {
                // don't render yourself
                if (this->session->getInfo().client_eid.has_value() && sharedObject->globalID == this->session->getInfo().client_eid.value()) {
                    //  TODO: Update the player eye level to an acceptable level
                    glm::vec3 pos = sharedObject->physics.getCenterPosition();
                    pos.y += PLAYER_EYE_LEVEL;
                    cam->updatePos(pos);
                    break;
                }
                auto lightPos = glm::vec3(0.0f, 10.0f, 0.0f);

                auto player_pos = sharedObject->physics.corner;

                this->player_model->setDimensions(sharedObject->physics.dimensions);
                this->player_model->translateAbsolute(player_pos);
                this->player_model->draw(
                    this->model_shader,
                    this->cam->getViewProj(),
                    this->cam->getPos(),
                    {},
                    true);
                this->drawBbox(sharedObject);
                break;
            }
            // case ObjectType::Enemy: {
            //     // warren bear is an enemy because why not
            //     auto lightPos = glm::vec3(-5.0f, 0.0f, 0.0f);
            //     auto bear_pos = sharedObject->physics.corner;

            //     this->bear_model->setDimensions(sharedObject->physics.dimensions);
            //     this->bear_model->translateAbsolute(bear_pos);
            //     this->bear_model->draw(
            //         this->model_shader,
            //         this->cam->getViewProj(),
            //         this->cam->getPos(),
            //         lightPos,
            //         true);
            //     this->drawBbox(sharedObject);
            //     break;
            // }
            case ObjectType::Slime: {
                auto cube = std::make_unique<Cube>(glm::vec3(0.0, 1.0f, 0.0f));
                cube->scaleAbsolute(sharedObject->physics.dimensions);
                cube->translateAbsolute(sharedObject->physics.getCenterPosition());
                cube->draw(this->cube_shader,
                    this->cam->getViewProj(),
                    this->cam->getPos(),
                    {},
                    true);
                break;
            }
            case ObjectType::SolidSurface: {
                // do not render ceiling if dungeon master
                if (!this->session->getInfo().is_dungeon_master.has_value()) {
                    break; // just in case this message wasn't received, don't crash
                }

                if (this->session->getInfo().is_dungeon_master.value()) {
                    if (sharedObject->solidSurface->surfaceType != SurfaceType::Ceiling) {
                        if (sharedObject->solidSurface->dm_highlight) {
                            this->cube_model->overrideSolidColor(glm::vec3(1.0f, 0.0f, 0.0f));
                        }
                        else {
                            this->cube_model->overrideSolidColor({});
                        }

                        this->cube_model->setDimensions(sharedObject->physics.dimensions);
                        this->cube_model->translateAbsolute(sharedObject->physics.getCenterPosition());
                        this->cube_model->draw(this->dm_cube_shader,
                            this->cam->getViewProj(),
                            this->cam->getPos(),
                            {}, // will make everything black if empty
                            true);
                    }
                }
                else {
                    this->cube_model->setDimensions(sharedObject->physics.dimensions);
                    this->cube_model->translateAbsolute(sharedObject->physics.getCenterPosition());
                    this->cube_model->draw(this->solid_surface_shader,
                        this->cam->getViewProj(),
                        this->cam->getPos(),
                        this->closest_light_sources,
                        true);
                }
                break;
            }
            case ObjectType::FakeWall: {
                glm::vec3 color;
                if (sharedObject->trapInfo->triggered) {
                    color = glm::vec3(0.4f, 0.5f, 0.7f);
                } else {
                    // off-color if not currently "visible"
                    // TODO: change to translucent
                    color = glm::vec3(0.5f, 0.6f, 0.8f);
                }
                auto cube = std::make_unique<Cube>(color);
                cube->scaleAbsolute(sharedObject->physics.dimensions);
                cube->translateAbsolute(sharedObject->physics.getCenterPosition());
                cube->draw(this->cube_shader,
                    this->cam->getViewProj(),
                    this->cam->getPos(),
                    {},
                    true);
                break;
            }
            case ObjectType::SpikeTrap: {
                auto cube = std::make_unique<Cube>(glm::vec3(1.0f, 0.1f, 0.1f));
                cube->scaleAbsolute( sharedObject->physics.dimensions);
                cube->translateAbsolute(sharedObject->physics.getCenterPosition());
                cube->draw(this->cube_shader,
                    this->cam->getViewProj(),
                    this->cam->getPos(),
                    {}, 
                    true);
                break;
            }
            case ObjectType::Torchlight: {
                // do not render torches if dungeon master
                if (!this->session->getInfo().is_dungeon_master.has_value()) {
                    break; // just in case this message wasn't received, don't crash
                }

                if (!this->session->getInfo().is_dungeon_master.value()) {
                    this->torchlight_model->translateAbsolute(sharedObject->physics.getCenterPosition());
                    this->torchlight_model->draw(
                        this->light_source_shader,
                        this->cam->getViewProj(),
                        this->cam->getPos(),
                        {},
                        true);
                }
                break;
            }
            case ObjectType::FireballTrap: {
                auto cube = std::make_unique<Cube>(glm::vec3(0.0f, 0.5f, 0.5f));
                cube->scaleAbsolute( sharedObject->physics.dimensions);
                cube->translateAbsolute(sharedObject->physics.getCenterPosition());
                cube->draw(this->cube_shader,
                    this->cam->getViewProj(),
                    this->cam->getPos(),
                    {},
                    true);
                break;
            }
            case ObjectType::ArrowTrap: {
                auto cube = std::make_unique<Cube>(glm::vec3(0.5f, 0.3f, 0.2f));
                cube->scaleAbsolute( sharedObject->physics.dimensions);
                cube->translateAbsolute(sharedObject->physics.getCenterPosition());
                cube->draw(this->cube_shader,
                    this->cam->getViewProj(),
                    this->cam->getPos(),
                    {},
                    true);
                break;
            }
            case ObjectType::Projectile: {  
                // TODO use model
                auto cube = std::make_unique<Cube>(glm::vec3(1.0f, 0.1f, 0.1f));
                cube->scaleAbsolute( sharedObject->physics.dimensions);
                cube->translateAbsolute(sharedObject->physics.getCenterPosition());
                cube->draw(this->cube_shader,
                    this->cam->getViewProj(),
                    this->cam->getPos(),
                    {},
                    true);
                break;
            }
            case ObjectType::FloorSpike: {
                auto cube = std::make_unique<Cube>(glm::vec3(0.0f, 1.0f, 0.0f));
                cube->scaleAbsolute( sharedObject->physics.dimensions);
                cube->translateAbsolute(sharedObject->physics.getCenterPosition());
                cube->draw(this->cube_shader,
                    this->cam->getViewProj(),
                    this->cam->getPos(),
                    {},
                    true);
                break;
            }
            case ObjectType::Orb: {
                if (!sharedObject->iteminfo->held && !sharedObject->iteminfo->used) {
                    glm::vec3 color = glm::vec3(0.0f, 0.7f, 1.0f);
                    auto cube = std::make_unique<Cube>(color);
                    cube->scaleAbsolute(sharedObject->physics.dimensions);
                    cube->translateAbsolute(sharedObject->physics.getCenterPosition());
                    cube->draw(this->cube_shader,
                        this->cam->getViewProj(),
                        this->cam->getPos(),
                        {},
                        true);
                }
                break;
            }
            case ObjectType::Potion: {
                if (!sharedObject->iteminfo->held && !sharedObject->iteminfo->used) {
                    glm::vec3 color;
                    if (sharedObject->modelType == ModelType::HealthPotion) {
                        color = glm::vec3(1.0f, 0.0f, 0.0f);
                    } else if (sharedObject->modelType == ModelType::NauseaPotion || sharedObject->modelType == ModelType::InvincibilityPotion) {
                        color = glm::vec3(1.0f, 0.5f, 0.0f);
                    } else if (sharedObject->modelType == ModelType::InvisibilityPotion) {
                        color = glm::vec3(0.2f, 0.2f, 0.2f);
                    }

                    auto cube = std::make_unique<Cube>(color);
                    cube->scaleAbsolute(sharedObject->physics.dimensions);
                    cube->translateAbsolute(sharedObject->physics.getCenterPosition());
                    cube->draw(this->cube_shader,
                        this->cam->getViewProj(),
                        this->cam->getPos(),
                        {},
                        true);
                }
                break;
            }
            case ObjectType::Spell: {
                if (!sharedObject->iteminfo->held && !sharedObject->iteminfo->used) {
                    glm::vec3 color;
                    if (sharedObject->modelType == ModelType::FireSpell) {
                        color = glm::vec3(0.9f, 0.1f, 0.0f);
                    }
                    else if (sharedObject->modelType == ModelType::HealSpell) {
                        color = glm::vec3(1.0f, 1.0f, 0.0f);
                    }

                    auto cube = std::make_unique<Cube>(color);
                    cube->scaleAbsolute(sharedObject->physics.dimensions);
                    cube->translateAbsolute(sharedObject->physics.getCenterPosition());
                    cube->draw(this->cube_shader,
                        this->cam->getViewProj(),
                        this->cam->getPos(),
                        {},
                        true);
                }
                break;
            }
            case ObjectType::TeleporterTrap: {
                auto cube = std::make_unique<Cube>(glm::vec3(0.0f, 1.0f, 1.0f));
                cube->scaleAbsolute( sharedObject->physics.dimensions);
                cube->translateAbsolute(sharedObject->physics.getCenterPosition());
                cube->draw(this->cube_shader,
                    this->cam->getViewProj(),
                    this->cam->getPos(),
                    {},
                    true);
                break;
            }
            case ObjectType::Exit: {
                auto cube = std::make_unique<Cube>(glm::vec3(0.0f, 0.0f, 0.0f));
                cube->scaleAbsolute( sharedObject->physics.dimensions);
                cube->translateAbsolute(sharedObject->physics.getCenterPosition());
                cube->draw(this->cube_shader,
                    this->cam->getViewProj(),
                    this->cam->getPos(),
                    {},
                    true);
                break;
            }
            default:
                break;
        }
    }
}

void Client::drawBbox(boost::optional<SharedObject> object) {
    if (this->config.client.draw_bboxes) {
        auto bbox_pos = object->physics.corner; 
        // for some reason the y axis of the bbox is off by half  
        // the dimension of the object. when trying getCenterPosition
        // it was off on the x axis. 
        bbox_pos.y += object->physics.dimensions.y / 2.0f; 

        auto object_bbox = std::make_unique<Cube>(glm::vec3(0.0f, 1.0f, 1.0f));
        object_bbox->scaleAbsolute(object->physics.dimensions);
        object_bbox->translateAbsolute(bbox_pos);
        object_bbox->draw(this->cube_shader,
            this->cam->getViewProj(),
            this->cam->getPos(),
            {},
            false);
    }
}

// callbacks - for Interaction
void Client::keyCallback(GLFWwindow *window, int key, int scancode, int action, int mods) {
    // Check for a key press.
    /* Store player EID for use in certain key handling */ 
    std::optional<EntityID> eid;

    if (this->session != nullptr && this->session->getInfo().client_eid.has_value()) {
        eid = this->session->getInfo().client_eid.value();
    }

    std::optional<bool> is_dm;

    if (this->session != nullptr && this->session->getInfo().is_dungeon_master.has_value()) {
        is_dm = this->session->getInfo().is_dungeon_master.value();
    }

    if (action == GLFW_PRESS) {
        switch (key) {
        case GLFW_KEY_ESCAPE:
            if (this->gameState.phase == GamePhase::GAME) {
                if (this->gui_state == GUIState::GAME_ESC_MENU) {
                    this->gui_state = GUIState::GAME_HUD;
                }
                else if (this->gui_state == GUIState::GAME_HUD) {
                    this->gui_state = GUIState::GAME_ESC_MENU;
                }
            }
            else if (this->gameState.phase == GamePhase::RESULTS) {
                if (this->gui_state == GUIState::RESULTS_SCREEN) {
                    this->gui_state = GUIState::GAME_ESC_MENU;
                }
                else if (this->gui_state == GUIState::GAME_ESC_MENU) {
                    this->gui_state = GUIState::RESULTS_SCREEN;
                }
            }
            this->gui.setCaptureKeystrokes(false);
            break;
        
        case GLFW_KEY_TAB:
            this->gui.setCaptureKeystrokes(true);
            break;
        
        case GLFW_KEY_BACKSPACE:
            this->gui.captureBackspace();
            Client::time_of_last_keystroke = getMsSinceEpoch();
            break;

        case GLFW_KEY_E:
            if (eid.has_value()) {
                if (is_dm.has_value() && !is_dm.value()) {
                    this->session->sendEventAsync(Event(eid.value(), EventType::UseItem, UseItemEvent(eid.value())));
                }
            }
            break;

        case GLFW_KEY_Q:
            if (eid.has_value()) {
                if (is_dm.has_value() && !is_dm.value()) {
                    this->session->sendEventAsync(Event(eid.value(), EventType::DropItem, DropItemEvent(eid.value())));
                }
            }
            break;

        case GLFW_KEY_1:
            if (eid.has_value()) {
                if (is_dm.has_value() && !is_dm.value()) {
                    this->session->sendEventAsync(Event(eid.value(), EventType::SelectItem, SelectItemEvent(eid.value(), 1)));
                }
            }
            break;

        case GLFW_KEY_2:
            if (eid.has_value()) {
                if (is_dm.has_value() && !is_dm.value()) {
                    this->session->sendEventAsync(Event(eid.value(), EventType::SelectItem, SelectItemEvent(eid.value(), 2)));
                }
            }
            break;

        case GLFW_KEY_3:
            if (eid.has_value()) {
                if (is_dm.has_value() && !is_dm.value()) {
                    this->session->sendEventAsync(Event(eid.value(), EventType::SelectItem, SelectItemEvent(eid.value(), 3)));
                }
            }
            break;

        case GLFW_KEY_4:
            if (eid.has_value()) {
                if (is_dm.has_value() && !is_dm.value()) {
                    this->session->sendEventAsync(Event(eid.value(), EventType::SelectItem, SelectItemEvent(eid.value(), 4)));
                }
            }
            break;
        case GLFW_KEY_RIGHT:
            if (eid.has_value()) {
                if (is_dm.has_value() && is_dm.value()) {
                    this->session->sendEventAsync(Event(eid.value(), EventType::SelectItem, SelectItemEvent(eid.value(), 1)));
                }
            }
            break;
        case GLFW_KEY_LEFT:
            if (eid.has_value()) {
                if (is_dm.has_value() && is_dm.value()) {
                    this->session->sendEventAsync(Event(eid.value(), EventType::SelectItem, SelectItemEvent(eid.value(), -1)));
                }
            }
            break;
        /* For movement keys (WASD), activate flags and use it to generate
         * movement in idleCallback() instead of sending individual events
         */  
        case GLFW_KEY_S:
            is_held_down = true;
            break;

        case GLFW_KEY_W:
            is_held_up = true;
            break;

        case GLFW_KEY_A:
            is_held_left = true;
            break;

        case GLFW_KEY_D:
            is_held_right = true;
            break;

        /* Space also uses a flag to constantly send events when key is held */
        case GLFW_KEY_SPACE:
            is_held_space = true;
            is_held_o = true;
            break;

        case GLFW_KEY_I: // zoom in
            is_held_i = true;
            break;
        case GLFW_KEY_O: // zoom out
            is_held_o = true;
            break;
        case GLFW_KEY_P: // to place or not to place
            if (is_pressed_p) {
                // unhighlight hover
                if (eid.has_value()) {
                    // nothing being placed, so the CellType we pass shouldn't matter!
                    this->session->sendEventAsync(Event(eid.value(), EventType::TrapPlacement, TrapPlacementEvent(eid.value(), this->world_pos, CellType::ArrowTrapUp, false, false)));
                }
            }
            is_pressed_p = !is_pressed_p;
            break;
        /* Send an event to start 'shift' movement (i.e. sprint) */
        case GLFW_KEY_LEFT_SHIFT:
            if (eid.has_value() && !this->session->getInfo().is_dungeon_master.value()) {
                this->session->sendEventAsync(Event(eid.value(), EventType::StartAction, StartActionEvent(eid.value(), glm::vec3(0.0f), ActionType::Sprint)));
            }
            is_held_i = true;
            break;

        default:
            break;
        }
    }

    if (action == GLFW_RELEASE) {
        switch (key) {
        case GLFW_KEY_S:
            is_held_down = false;
            break;

        case GLFW_KEY_W:
            is_held_up = false;
            break;

        case GLFW_KEY_A:
            is_held_left = false;
            break;

        case GLFW_KEY_D:
            is_held_right = false;
            break;
            
        case GLFW_KEY_SPACE:
            is_held_space = false;
            is_held_o = false;
            break;

        case GLFW_KEY_LEFT_SHIFT:
            if (eid.has_value() && !this->session->getInfo().is_dungeon_master.value()) {
                this->session->sendEventAsync(Event(eid.value(), EventType::StopAction, StopActionEvent(eid.value(), glm::vec3(0.0f), ActionType::Sprint)));
            }
            is_held_i = false;
            break;

        case GLFW_KEY_O: // zoom out
            is_held_o = false;
            break;
        case GLFW_KEY_I: // zoom out
            is_held_i = false;
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
                this->gui.captureBackspace();
            }
        }
    }
}

void Client::mouseCallback(GLFWwindow* window, double xposIn, double yposIn) { // cppcheck-suppress constParameterPointer
    auto new_mouse_xpos = static_cast<float>(xposIn);
    auto new_mouse_ypos = static_cast<float>(yposIn);

    if (new_mouse_xpos == mouse_xpos && new_mouse_ypos == mouse_ypos)
        return;

    mouse_xpos = new_mouse_xpos;
    mouse_ypos = new_mouse_ypos;

    if (is_pressed_p) {
        auto eid = this->session->getInfo().client_eid.value();

        // the actual trap doesn't matter, this is just for highlighting purposes
        this->session->sendEventAsync(Event(eid, EventType::TrapPlacement, TrapPlacementEvent(eid, this->world_pos, CellType::FloorSpikeFull, true, false)));
    }
}

void Client::setWorldPos() {
    float center_mouse_ypos = window_height / 2;
    float center_mouse_xpos = window_width / 2;

    GLfloat winZ;
    glReadPixels(center_mouse_xpos, window_height - center_mouse_ypos, 1, 1, GL_DEPTH_COMPONENT, GL_FLOAT, &winZ);
    glm::vec3 win(center_mouse_xpos, window_height - center_mouse_ypos, winZ);

    GLint viewport[4];
    glGetIntegerv(GL_VIEWPORT, viewport);
    glm::vec4 vport(viewport[0], viewport[1], viewport[2], viewport[3]);

    this->world_pos = glm::unProject(win, glm::mat4(1.0f), this->cam->getProjection() * this->cam->getView(), vport);
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
    gui.captureKeystroke(static_cast<char>(codepoint));
    Client::time_of_last_keystroke = getMsSinceEpoch();
}

glm::vec2 Client::getWindowSize() {
    return glm::vec2(Client::window_width, Client::window_height);
}

time_t Client::getTimeOfLastKeystroke() {
    return Client::time_of_last_keystroke;
}