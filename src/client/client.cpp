#include "client/client.hpp"

Client::Client() {

}

Client::~Client() {

}

// Remember to do error message output for later
int Client::start() {
    GLFWwindow *window;

    /* Initialize the library */
    if (!glfwInit())
        return -1;

    /* Create a windowed mode window and its OpenGL context */
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    window = glfwCreateWindow(640, 480, "Hello World", NULL, NULL);
    if (!window)
    {
        glfwTerminate();
        return -1;
    }

    /* Make the window's context current */
    glfwMakeContextCurrent(window);

    // std::cout << "shader version: " << glGetString(GL_SHADING_LANGUAGE_VERSION) << std::endl;

    /* Load shader programs */
    std::cout << "loading shader" << std::endl;
    GLuint shaderProgram = LoadShaders("../src/client/shaders/shader.vert", "../src/client/shaders/shader.frag");

    // Check the shader program.
    if (!shaderProgram) {
        std::cerr << "Failed to initialize shader program" << std::endl;
        return false;
    }

    /* Initialize GLAD */
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
        return -1;

    Cube* c = new Cube();

    /* Loop until the user closes the window */
    while (!glfwWindowShouldClose(window))
    {
        /* Render here */
        glClear(GL_COLOR_BUFFER_BIT);
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

        /* Swap front and back buffers */
        glfwSwapBuffers(window);
        c->draw(shaderProgram);

        /* Poll for and process events */
        glfwPollEvents();
    }

    glfwTerminate();

    glDeleteProgram(shaderProgram);
    return 0;
}
