#include <iostream>
#include <chrono>


#include <boost/asio/io_context.hpp>
#include <SFML/Audio.hpp>

#include "client/client.hpp"
#include "shared/utilities/rng.hpp"
#include "shared/utilities/config.hpp"
#include "shared/utilities/root_path.hpp"
#include "client/sound.hpp"

#include "shared/utilities/root_path.hpp"
#include "client/gui/img/img.hpp"
#include "stb_image.h"

using namespace std::chrono_literals;

void error_callback(int error, const char* description) {
    // Print error.
    std::cerr << description << std::endl;
}

void set_callbacks(GLFWwindow* window, Client* client) {
    // Set the error callback.
    glfwSetErrorCallback(error_callback);

    // Set the window resize callback.
    // glfwSetWindowSizeCallback(window, client.resizeCallback);

    // Set the key callback.
    glfwSetKeyCallback(window, Client::keyCallback);

    // Set the mouse and cursor callbacks
    glfwSetMouseButtonCallback(window, Client::mouseButtonCallback);
    glfwSetCursorPosCallback(window, Client::mouseCallback);

    glfwSetCharCallback(window, Client::charCallback);
}

void set_opengl_settings(GLFWwindow* window) {
    // Enable depth buffering.
    glEnable(GL_DEPTH_TEST);

    // Related to shaders and z value comparisons for the depth buffer.
    glDepthFunc(GL_LEQUAL);

    // Set polygon drawing mode to fill front and back of each polygon.
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

    // Set clear color to black.
    glClearColor(0.8f, 0.8f, 0.8f, 1.0f);

    // Set cursor position to (0, 0)
    glfwSetCursorPos(window, 0, 0);
}

int main(int argc, char* argv[])
{
    auto config = GameConfig::parse(argc, argv);
    boost::asio::io_context context;
    Client client(context, config);

    if (client.init() == -1) {
        exit(EXIT_FAILURE);
    }

    GLFWwindow* window = client.getWindow();
    if (!window) exit(EXIT_FAILURE);

    glfwSetWindowUserPointer(window, &client);

    // Setup callbacks.
    set_callbacks(window, &client);
    // Setup OpenGL settings.
    set_opengl_settings(window);

    boost::filesystem::path soundFilepath = getRepoRoot() / "assets/sounds/piano.wav";

    sf::SoundBuffer buffer;
    if (!buffer.loadFromFile(soundFilepath.string()))
        return -1;

    sf::Sound sound;
    sound.setBuffer(buffer);

    sound.setLoop(true);

    // sound.play();

    float vertices[] = {
        // positions          // colors           // texture coords
         0.5f,  0.5f, 0.0f,   1.0f, 0.0f, 0.0f,   1.0f, 1.0f, // top right
         0.5f, -0.5f, 0.0f,   0.0f, 1.0f, 0.0f,   1.0f, 0.0f, // bottom right
        -0.5f, -0.5f, 0.0f,   0.0f, 0.0f, 1.0f,   0.0f, 0.0f, // bottom left
        -0.5f,  0.5f, 0.0f,   1.0f, 1.0f, 0.0f,   0.0f, 1.0f  // top left 
    };
    unsigned int indices[] = {
        0, 1, 3, // first triangle
        1, 2, 3  // second triangle
    };
    unsigned int VBO, VAO, EBO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    // position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    // color attribute
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    // texture coord attribute
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
    glEnableVertexAttribArray(2);


    // load and create a texture 
    // -------------------------
    unsigned int texture1;
    // texture 1
    // ---------
    glGenTextures(1, &texture1);
    glBindTexture(GL_TEXTURE_2D, texture1); 
     // set the texture wrapping parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);	// set texture wrapping to GL_REPEAT (default wrapping method)
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    // set texture filtering parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    // load image, create texture and generate mipmaps
    int width, height, nrChannels;
    stbi_set_flip_vertically_on_load(true); // tell stb_image.h to flip loaded texture's on the y-axis.
    // The FileSystem::getPath(...) is part of the GitHub repository so we can find files on any IDE/platform; replace it with your own image path.
    auto img_root = getRepoRoot() / "assets/imgs";

    std::cout << (img_root / "awesomeface.png").string() << std::endl;
    unsigned char *data = stbi_load((img_root / "awesomeface.png").string().c_str(), &width, &height, &nrChannels, 0);
    if (data)
    {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
    }
    else
    {
        std::cout << "Failed to load texture" << std::endl;
    }
    std::cout << "data: " << data << std::endl;
    stbi_image_free(data);

    auto shader_path = getRepoRoot() / "src" / "client" / "shaders";
    auto imgShaderProgram = LoadShaders((shader_path / "test.vert").string(), (shader_path / "test.frag").string());
    if (!imgShaderProgram) {
        std::cerr << "Failed to initialize test shader program" << std::endl;
        return false;
    }

    // tell opengl for each sampler to which texture unit it belongs to (only has to be done once)
    // -------------------------------------------------------------------------------------------
    glUseProgram(imgShaderProgram); // don't forget to activate/use the shader before setting uniforms!
    // either set it manually like so:
    glUniform1i(glGetUniformLocation(imgShaderProgram, "texture1"), texture1);
    // or set it via the texture class

    std::cout << texture1 << std::endl;

    // render loop
    // -----------
    while (!glfwWindowShouldClose(window))
    {
        // input
        // -----
        // processInput(window);

        // render
        // ------
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        // bind textures on corresponding texture units
        glActiveTexture(GL_TEXTURE3);
        glBindTexture(GL_TEXTURE_2D, texture1);

        // render container
        glUseProgram(imgShaderProgram);
        glBindVertexArray(VAO);
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

        // glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
        // -------------------------------------------------------------------------------
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // optional: de-allocate all resources once they've outlived their purpose:
    // ------------------------------------------------------------------------
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO);

    // glfw: terminate, clearing all previously allocated GLFW resources.
    // ------------------------------------------------------------------
    glfwTerminate();
    return 0;

    // Loop while GLFW window should stay open.
    while (!glfwWindowShouldClose(window)) {
        // Main render display callback. Rendering of objects is done here.
        client.displayCallback();

        // Idle callback. Updating objects, etc. can be done here.
        client.idleCallback(context);
    }

    client.cleanup();

    // Destroy the window.
    glfwDestroyWindow(window);
    // Terminate GLFW.
    glfwTerminate();

    exit(EXIT_SUCCESS);

    return 0;
}

// #include <iostream>
// #include <chrono>


// #include <boost/asio/io_context.hpp>
// #include <SFML/Audio.hpp>

// #include "client/client.hpp"
// #include "shared/utilities/rng.hpp"
// #include "shared/utilities/config.hpp"
// #include "shared/utilities/root_path.hpp"
// #include "client/sound.hpp"

// #include "shared/utilities/root_path.hpp"
// #include "client/gui/img/img.hpp"
// #include "stb_image.h"

// void framebuffer_size_callback(GLFWwindow* window, int width, int height);
// void processInput(GLFWwindow *window);

// // settings
// const unsigned int SCR_WIDTH = 800;
// const unsigned int SCR_HEIGHT = 600;

// int main()
// {
//     // glfw: initialize and configure
//     // ------------------------------
//     glfwInit();
//     glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
//     glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
//     glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

// #ifdef __APPLE__
//     glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
// #endif

//     // glfw window creation
//     // --------------------
//     GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "LearnOpenGL", NULL, NULL);
//     if (window == NULL)
//     {
//         std::cout << "Failed to create GLFW window" << std::endl;
//         glfwTerminate();
//         return -1;
//     }
//     glfwMakeContextCurrent(window);
//     glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

//     // glad: load all OpenGL function pointers
//     // ---------------------------------------
//     // if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
//     // {
//     //     std::cout << "Failed to initialize GLAD" << std::endl;
//     //     return -1;
//     // }
//     GLenum err = glewInit() ; 
//     if (GLEW_OK != err) { 
//         std::cerr << "Error loading GLEW: " << glewGetString(err) << std::endl; 
//         return false;
//     } 

//     // build and compile our shader zprogram
//     // ------------------------------------
//     // Shader ourShader("/Users/dmin/Documents/Homework/Spring_2024/LearnOpenGL/src/1.getting_started/4.1.textures/4.1.texture.vs", "/Users/dmin/Documents/Homework/Spring_2024/LearnOpenGL/src/1.getting_started/4.1.textures/4.1.texture.fs"); 
//     auto shaderprog = LoadShaders("/Users/dmin/Documents/Homework/Spring_2024/group3/src/client/shaders/test.vert", "/Users/dmin/Documents/Homework/Spring_2024/group3/src/client/shaders/test.frag");

//     // set up vertex data (and buffer(s)) and configure vertex attributes
//     // ------------------------------------------------------------------
//     float vertices[] = {
//         // positions          // colors           // texture coords
//          0.5f,  0.5f, 0.0f,   1.0f, 0.0f, 0.0f,   1.0f, 1.0f, // top right
//          0.5f, -0.5f, 0.0f,   0.0f, 1.0f, 0.0f,   1.0f, 0.0f, // bottom right
//         -0.5f, -0.5f, 0.0f,   0.0f, 0.0f, 1.0f,   0.0f, 0.0f, // bottom left
//         -0.5f,  0.5f, 0.0f,   1.0f, 1.0f, 0.0f,   0.0f, 1.0f  // top left 
//     };
//     unsigned int indices[] = {  
//         0, 1, 3, // first triangle
//         1, 2, 3  // second triangle
//     };
//     unsigned int VBO, VAO, EBO;
//     glGenVertexArrays(1, &VAO);
//     glGenBuffers(1, &VBO);
//     glGenBuffers(1, &EBO);

//     glBindVertexArray(VAO);

//     glBindBuffer(GL_ARRAY_BUFFER, VBO);
//     glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

//     glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
//     glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

//     // position attribute
//     glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
//     glEnableVertexAttribArray(0);
//     // color attribute
//     glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
//     glEnableVertexAttribArray(1);
//     // texture coord attribute
//     glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
//     glEnableVertexAttribArray(2);


//     // load and create a texture 
//     // -------------------------
//     unsigned int texture;
//     glGenTextures(1, &texture);
//     glBindTexture(GL_TEXTURE_2D, texture); // all upcoming GL_TEXTURE_2D operations now have effect on this texture object
//     // set the texture wrapping parameters
//     glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);	// set texture wrapping to GL_REPEAT (default wrapping method)
//     glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
//     // set texture filtering parameters
//     glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
//     glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
//     // load image, create texture and generate mipmaps
//     int width, height, nrChannels;
//     // The FileSystem::getPath(...) is part of the GitHub repository so we can find files on any IDE/platform; replace it with your own image path.
//     unsigned char *data = stbi_load("/Users/dmin/Documents/Homework/Spring_2024/group3/assets/imgs/awesomeface.png", &width, &height, &nrChannels, 0);
//     // std::cout << "data: " << data << std::endl;
//     if (data)
//     {
//         glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
//         glGenerateMipmap(GL_TEXTURE_2D);
//     }
//     else
//     {
//         std::cout << "Failed to load texture" << std::endl;
//     }
//     stbi_image_free(data);


//     // render loop
//     // -----------
//     while (!glfwWindowShouldClose(window))
//     {
//         // input
//         // -----
//         processInput(window);

//         // render
//         // ------
//         glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
//         glClear(GL_COLOR_BUFFER_BIT);

//         // bind Texture
//         glBindTexture(GL_TEXTURE_2D, texture);

//         // render container
//         // ourShader.use();
//         glUseProgram(shaderprog);
//         glBindVertexArray(VAO);
//         glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

//         // glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
//         // -------------------------------------------------------------------------------
//         glfwSwapBuffers(window);
//         glfwPollEvents();
//     }

//     // optional: de-allocate all resources once they've outlived their purpose:
//     // ------------------------------------------------------------------------
//     glDeleteVertexArrays(1, &VAO);
//     glDeleteBuffers(1, &VBO);
//     glDeleteBuffers(1, &EBO);

//     // glfw: terminate, clearing all previously allocated GLFW resources.
//     // ------------------------------------------------------------------
//     glfwTerminate();
//     return 0;
// }

// // process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// // ---------------------------------------------------------------------------------------------------------
// void processInput(GLFWwindow *window)
// {
//     if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
//         glfwSetWindowShouldClose(window, true);
// }

// // glfw: whenever the window size changed (by OS or user resize) this callback function executes
// // ---------------------------------------------------------------------------------------------
// void framebuffer_size_callback(GLFWwindow* window, int width, int height)
// {
//     // make sure the viewport matches the new window dimensions; note that width and 
//     // height will be significantly larger than specified on retina displays.
//     glViewport(0, 0, width, height);
// }
