#include "client/gui/gui.hpp"

#include "client/client.hpp"
#include "client/shader.hpp"



namespace gui::widget {

std::unique_ptr<Shader> StaticImg::shader = nullptr;

StaticImg::StaticImg(glm::vec2 origin, gui::img::Img img, int size):
    Widget(Type::StaticImg, origin), img(img)
{
    this->size = size;
    this->width = img.width * size;
    this->height = img.height * size;
    this->texture_id = img.texture_id;
}

StaticImg::StaticImg(glm::vec2 origin, gui::img::Img img):
    Widget(Type::StaticImg, origin), img(img)
{
    this->size = 1.0f;
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
    // ⚠ SUS SHIT ⚠
    float width_percent = (2.0f / (WINDOW_WIDTH)) * (img.width) * size;
    float height_percent = (2.0f / (WINDOW_HEIGHT)) * (img.height) * size;
    glm::vec2 bottom_left = (2.0f * (origin / glm::vec2(WINDOW_WIDTH, WINDOW_HEIGHT))) - glm::vec2(1.0f, 1.0f);

    float vertices[] = {
        // positions          // colors           // texture coords
        bottom_left.x + width_percent,  bottom_left.y + height_percent, 0.0f,   1.0f, 0.0f, 0.0f,   1.0f, 1.0f, // top right
        bottom_left.x + width_percent, bottom_left.y, 0.0f,   0.0f, 1.0f, 0.0f,   1.0f, 0.0f, // bottom right
        bottom_left.x, bottom_left.y, 0.0f,   0.0f, 0.0f, 1.0f,   0.0f, 0.0f, // bottom left
        bottom_left.x, bottom_left.y + height_percent, 0.0f,   1.0f, 1.0f, 0.0f,   0.0f, 1.0f  // top left 
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

    glDisable(GL_CULL_FACE);
    StaticImg::shader->use();

    // glm::mat4 projection = GUI_PROJECTION_MATRIX();
    // shader->setMat4("projection", projection);
    // glm::mat4 transform = glm::mat4(1.0f);
    // // transform = glm::translate(transform, glm::vec3(this->origin.x, this->origin.y, 0));
    // shader->setMat4("transform", transform);

    // glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, this->texture_id);
    glBindVertexArray(quadVAO);
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

    glBindVertexArray(0);
    glBindTexture(GL_TEXTURE_2D, 0);
    glUseProgram(0);

    glEnable(GL_CULL_FACE);
}

}
