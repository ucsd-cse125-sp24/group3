#include "client/gui/gui.hpp"

#include "client/client.hpp"



namespace gui::widget {

GLuint StaticImg::shader = 0;

StaticImg::StaticImg(glm::vec2 origin, gui::img::Img img):
    Widget(Type::StaticImg, origin)
{
    // // configure VAO/VBO
    // unsigned int VBO;
    // // there might be some mismatch here because this might be assuming that the top left
    // // corner is 0,0 when we are specifying origin by bottom left coordinate
    // float vertices[] = {
    //     // pos // tex
    //     0.0f, 1.0f, 0.0f, 1.0f,
    //     1.0f, 0.0f, 1.0f, 0.0f,
    //     0.0f, 0.0f, 0.0f, 0.0f,
    //     0.0f, 1.0f, 0.0f, 1.0f,
    //     1.0f, 1.0f, 1.0f, 1.0f,
    //     1.0f, 0.0f, 1.0f, 0.0f
    // };
    // glGenVertexArrays(1, &quadVAO);
    // glGenBuffers(1, &VBO);

    // glBindBuffer(GL_ARRAY_BUFFER, VBO);
    // glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    // glBindVertexArray(quadVAO);
    // glEnableVertexAttribArray(0);
    // glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float),(void*)0);
    // glBindBuffer(GL_ARRAY_BUFFER, 0);
    // glBindVertexArray(0);

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

    glGenVertexArrays(1, &quadVAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    glBindVertexArray(quadVAO);

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


    this->width = img.width;
    this->height = img.height;
    this->texture_id = img.texture_id;
}

StaticImg::StaticImg(gui::img::Img img):
    StaticImg({0.0f, 0.0f}, img) {}

StaticImg::~StaticImg() {
    glDeleteVertexArrays(1, &quadVAO);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO);
}

void StaticImg::render() {
    glUseProgram(StaticImg::shader);
    glUniform1i(glGetUniformLocation(StaticImg::shader, "texture1"), this->texture_id);

    glm::mat4 model = glm::mat4(1.0f);
    // model = glm::translate(model, glm::vec3(origin, 0.0f));
    // model = glm::translate(model, glm::vec3(0.5*width, 0.5*height, 0.0));
    // model = glm::rotate(model, glm::radians(0.0f), glm::vec3(0.0, 0.0, 1.0));
    // model = glm::translate(model, glm::vec3(-0.5*width, -0.5*height, 0.0));
    // model = glm::scale(model, glm::vec3(glm::vec2(width, height), 1.0f));
    // glUniformMatrix4fv(glGetUniformLocation(StaticImg::shader, "projection"), 1, false, reinterpret_cast<float*>(&GUI::projection));
    // glUniformMatrix4fv(glGetUniformLocation(StaticImg::shader, "model"), 1, false, reinterpret_cast<float*>(&model));
    // glUniform3f(glGetUniformLocation(StaticImg::shader, "spriteColor"), 1.0f, 1.0f, 1.0f);

    // glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_2D, this->texture_id);
    glBindVertexArray(quadVAO);
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

    glBindVertexArray(0);
    glBindTexture(GL_TEXTURE_2D, 0);
    glUseProgram(0);
}

}
