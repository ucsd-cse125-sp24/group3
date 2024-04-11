#include "client/client.hpp"

Client::Client() {

}

// Remember to do error message output for later
int Client::start() {
    GLFWwindow *window;

    /* Initialize the library */
    if (!glfwInit())
        return -1;

    /* Create a windowed mode window and its OpenGL context */
    window = glfwCreateWindow(640, 480, "Hello World", NULL, NULL);
    if (!window)
    {
        glfwTerminate();
        return -1;
    }

    /* Make the window's context current */
    glfwMakeContextCurrent(window);

    /* Initialize GLAD */
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
        return -1;

    /* Vector Math */
    glm::vec3 v1(1.0, 2.0, 3.0);
    glm::vec3 v2(10.0, 11.0, 2.0);
    glm::vec3 v = v1 + v2;
    std::cout << "Vector Result: " << v.x << " " << v.y << " " << v.z << std::endl;

    /* Loop until the user closes the window */
    while (!glfwWindowShouldClose(window))
    {
        /* Render here */
        glClear(GL_COLOR_BUFFER_BIT);
        glClearColor(0.0f, 1.0f, 0.0f, 1.0f);

        /* Swap front and back buffers */
        glfwSwapBuffers(window);

        /* Poll for and process events */
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}
