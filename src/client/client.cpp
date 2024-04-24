#include "client/client.hpp"
#include <GLFW/glfw3.h>

#include <boost/asio/ip/tcp.hpp>
#include <boost/asio/io_context.hpp>
#include <boost/archive/text_iarchive.hpp>
#include <iostream>
#include <thread>

#include "shared/game/event.hpp"
#include "shared/network/constants.hpp"
#include "shared/network/packet.hpp"
#include "shared/utilities/config.hpp"

using namespace boost::asio::ip;
using namespace std::chrono_literals;

Client::Client(boost::asio::io_context& io_context, GameConfig config):
    resolver(io_context),
    socket(io_context),
    config(config),
    gameState(GamePhase::TITLE_SCREEN, config)
{
}

void Client::connectAndListen(std::string ip_addr) {
    this->endpoints = resolver.resolve(ip_addr, std::to_string(config.network.server_port));
    this->session = std::make_shared<Session>(std::move(this->socket), SessionInfo {
        .client_name = this->config.client.default_name,
        .client_eid = {}
    });

    this->session->connectTo(this->endpoints);

    auto packet = PackagedPacket::make_shared(PacketType::ClientDeclareInfo,
        ClientDeclareInfoPacket { .player_name = config.client.default_name });

    this->session->sendPacketAsync(packet);

    this->session->startListen();
}

Client::~Client() {

}

int Client::init() {
    /* Initialize the library */
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

    // https://stackoverflow.com/questions/12329082/glcreateshader-is-crashing#comment43358404_23541855
    #ifndef __APPLE__ // GLew not needed on OSX systems
    GLenum err = glewInit() ; 
    if (GLEW_OK != err) { 
        std::cerr << "Error: " << glewGetString(err) << std::endl; 
    } 
    #endif

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

    return 0;
}

// Remember to do error message output for later
int Client::start(boost::asio::io_context& context) {
    init();

    // Constrain framerate
    /* Loop until the user closes the window */
    while (!glfwWindowShouldClose(window))
    {
        processClientInput();
        processServerInput(context);
        /* Render here */
        glClearColor(0.8f, 0.8f, 0.8f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        if (this->gameState.phase == GamePhase::GAME) {
            this->draw();
        }

        /* Swap front and back buffers */
        glfwSwapBuffers(window);
        /* Poll for and process events */
        glfwPollEvents();

        // std::this_thread::sleep_for(std::chrono::seconds(1));
    }

    glfwTerminate();

    glDeleteProgram(shaderProgram);
    return 0;
}

void Client::processClientInput() {
    std::optional<glm::vec3> movement;
    if(glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
    if(glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
    if(glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS)
       movement = glm::vec3(cubeMovementDelta, 0.0f, 0.0f);
    if(glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS)
       movement = glm::vec3(-cubeMovementDelta, 0.0f, 0.0f);
    if(glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS)
        movement = glm::vec3(0.0f, cubeMovementDelta, 0.0f);
    if(glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS)
        movement = glm::vec3(0.0f, -cubeMovementDelta, 0.0f);

    if (movement.has_value()) {
        auto eid = 0; 
        this->session->sendEventAsync(Event(eid, EventType::MoveRelative, MoveRelativeEvent(eid, movement.value())));
    }
}

void Client::processServerInput(boost::asio::io_context& context) {
    context.run_for(30ms);

    // probably want to put rendering logic inside of client, so that this main function
    // mimics the server one where all of the important logic is done inside of a run command
    // But this is a demo of how you could use the client session to get information from
    // the game state

    for (Event event : this->session->getEvents()) {
        std::cout << "Event Received: " << event << std::endl;
        if (event.type == EventType::LoadGameState) {
            this->gameState = boost::get<LoadGameStateEvent>(event.data).state;

            // for (const auto& [eid, player] : data.state.getLobbyPlayers()) {
            //     std::cout << "\tPlayer " << eid << ": " << player << "\n";
            // }
            // std::cout << "\tThere are " <<
            //     data.state.getLobbyMaxPlayers() - data.state.getLobbyPlayers().size() <<
            //     " slots remaining in this lobby\n";
        }
    }
}

void Client::draw() {
    //for(const Object& obj: this->gameState.getObjects()) {
    //    std::cout << "got an object" << std::endl;
    //    // tmp: all objects are cubes
    //    Cube* cube = new Cube();
    //    cube->update(obj.position);
    //    cube->draw(this->shaderProgram);
    //}
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
