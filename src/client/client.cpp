#include "client/client.hpp"
#include <GLFW/glfw3.h>

Client::Client() {

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

    ///* Initialize GLAD */
    //if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    //    return -1;

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

    cube = new Cube();

    return 0;
}

// Remember to do error message output for later
int Client::start() {
    init();

    // Constrain framerate
    /* Loop until the user closes the window */
    while (!glfwWindowShouldClose(window))
    {
        processInput();
        /* Render here */
        glClearColor(0.8f, 0.8f, 0.8f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        /* Swap front and back buffers */
        cube->draw(shaderProgram);

        /* Poll for and process events */
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwTerminate();

    glDeleteProgram(shaderProgram);
    return 0;
}

void Client::processInput() {
    if(glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
    if(glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
    if(glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS)
        cube->update_delta(glm::vec3(cubeMovementDelta, 0.0f, 0.0f));
    if(glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS)
        cube->update_delta(glm::vec3(-cubeMovementDelta, 0.0f, 0.0f));
    if(glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS)
        cube->update_delta(glm::vec3(0.0f, cubeMovementDelta, 0.0f));
    if(glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS)
        cube->update_delta(glm::vec3(0.0f, -cubeMovementDelta, 0.0f));
}
