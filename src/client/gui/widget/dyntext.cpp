#include "client/gui/widget/dyntext.hpp"
#include "client/gui/font/font.hpp"
#include "client/gui/font/loader.hpp"
#include "client/core.hpp"
#include "client/client.hpp"
#include "client/shader.hpp"

#include <string>
#include <algorithm>
#include <memory>
#include <iostream>

namespace gui::widget {

std::unique_ptr<Shader> DynText::shader = nullptr;

DynText::DynText(glm::vec2 origin, const std::string& text, 
    std::shared_ptr<gui::font::Loader> fonts, DynText::Options options):
    text(text), options(options), fonts(fonts), Widget(Type::DynText, origin)
{
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 6 * 4, NULL, GL_DYNAMIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), 0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    // Calculate size of string of text
    this->width = 0;
    this->height = 0;

    float scale = font::getScaleFactor(this->options.size);

    for (int i = 0; i < text.size(); i++) {
        font::Character ch = this->fonts->loadChar(this->text[i], this->options.font);
        this->height = std::max(this->height, static_cast<std::size_t>(ch.size.y * scale));
        if (i != text.size() - 1 && i != 0) {
            this->width += (ch.advance >> 6) * scale;
        } else {
            this->width += ch.size.x * scale;
        }
    }
}

DynText::DynText(const std::string& text, std::shared_ptr<gui::font::Loader> fonts, DynText::Options options):
    DynText({0.0f, 0.0f}, text, fonts, options) {}

void DynText::render() {
    DynText::shader->use();

    auto projection = GUI_PROJECTION_MATRIX();
    DynText::shader->setMat4("projection", projection);
    auto color = this->options.color;
    DynText::shader->setVec3("textColor", color);
    glBindVertexArray(VAO);

    float x = this->origin.x;
    float y = this->origin.y;

    // iterate through all characters
    for (const char& c : this->text)
    {
        font::Character ch = this->fonts->loadChar(c, this->options.font);

        float scale = font::getScaleFactor(this->options.size);

        float xpos = x + ch.bearing.x * scale;
        float ypos = y - (ch.size.y - ch.bearing.y) * scale;

        float w = ch.size.x * scale;
        float h = ch.size.y * scale;
        // update VBO for each character
        float vertices[6][4] = {
            { xpos,     ypos + h,   0.0f, 0.0f },            
            { xpos,     ypos,       0.0f, 1.0f },
            { xpos + w, ypos,       1.0f, 1.0f },

            { xpos,     ypos + h,   0.0f, 0.0f },
            { xpos + w, ypos,       1.0f, 1.0f },
            { xpos + w, ypos + h,   1.0f, 0.0f }           
        };
        // render glyph texture over quad
        glBindTexture(GL_TEXTURE_2D, ch.texture_id);
        // update content of VBO memory
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices); 
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        // render quad
        glDrawArrays(GL_TRIANGLES, 0, 6);
        // now advance cursors for next glyph (note that advance is number of 1/64 pixels)
        x += (ch.advance >> 6) * scale; // bitshift by 6 to get value in pixels (2^6 = 64)
    }

    glBindVertexArray(0);
    glBindTexture(GL_TEXTURE_2D, 0);
    glUseProgram(0);
}

void DynText::changeColor(font::Color new_color) {
    this->options.color = font::getRGB(new_color);
}

}