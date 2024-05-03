#include "client/gui/gui.hpp"

#include "client/client.hpp"

namespace gui::widget {

GLuint StaticImg::shader = 0;

StaticImg::StaticImg(glm::vec2 origin, gui::img::Img img):
    Widget(Type::StaticImg, origin)
{
    // configure VAO/VBO
    unsigned int VBO;
    // there might be some mismatch here because this might be assuming that the top left
    // corner is 0,0 when we are specifying origin by bottom left coordinate
    float vertices[] = {
        // pos // tex
        0.0f, 1.0f, 0.0f, 1.0f,
        1.0f, 0.0f, 1.0f, 0.0f,
        0.0f, 0.0f, 0.0f, 0.0f,
        0.0f, 1.0f, 0.0f, 1.0f,
        1.0f, 1.0f, 1.0f, 1.0f,
        1.0f, 0.0f, 1.0f, 0.0f
    };
    glGenVertexArrays(1, &quadVAO);
    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    glBindVertexArray(quadVAO);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float),(void*)0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    this->width = img.width;
    this->height = img.height;
}

StaticImg::StaticImg(gui::img::Img img):
    StaticImg({0.0f, 0.0f}, img) {}

void StaticImg::render() {
    glUseProgram(StaticImg::shader);

    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(origin, 0.0f));
    model = glm::translate(model, glm::vec3(0.5*width, 0.5*height, 0.0));
    model = glm::rotate(model, glm::radians(0.0f), glm::vec3(0.0, 0.0, 1.0));
    model = glm::translate(model, glm::vec3(-0.5*width, -0.5*height, 0.0));
    model = glm::scale(model, glm::vec3(glm::vec2(width, height), 1.0f));
    glUniformMatrix4fv(glGetUniformLocation(StaticImg::shader, "projection"), 1, false, reinterpret_cast<float*>(&GUI::projection));
    glUniformMatrix4fv(glGetUniformLocation(StaticImg::shader, "model"), 1, false, reinterpret_cast<float*>(&model));
    glUniform3f(glGetUniformLocation(StaticImg::shader, "spriteColor"), 1.0f, 1.0f, 1.0f);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, img.texture_id);
    glBindVertexArray(quadVAO);
    glDrawArrays(GL_TRIANGLES, 0, 6);

    glUseProgram(0);
}

}
