#include "client/gui/widget/dyntext.hpp"
#include "client/gui/font/font.hpp"
#include "client/gui/font/loader.hpp"
#include "client/core.hpp"
#include "client/client.hpp"

#include <string>
#include <algorithm>
#include <memory>
#include <iostream>

namespace gui::widget {

DynText::DynText(std::string text, std::shared_ptr<gui::font::Loader> fonts,
    DynText::Options options):
    text(text), options(options), fonts(fonts), Widget(Type::DynText)
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
    for (int i = 0; i < text.size(); i++) {
        font::Character ch = this->fonts->loadChar(this->text[i], this->options.font, this->options.font_size);
        this->height = std::max(this->height, static_cast<std::size_t>(ch.size.y));
        if (i != text.size() - 1 && i != 0) {
            this->width += ch.advance / 64.0;
        } else {
            this->width += ch.size.x;
        }
    }
}

DynText::DynText(std::string text, std::shared_ptr<gui::font::Loader> fonts):
    DynText(text, fonts, DynText::Options {
        .font {font::Font::TEXT},
        .font_size {font::FontSizePx::MEDIUM},
        .color {font::getRGB(font::FontColor::BLACK)},
        .scale {1.0},
    })
{
    // let the default values take over for options
}

void DynText::render(GLuint shader, float x, float y) {
    glUseProgram(shader);

    // todo move to gui
    glm::mat4 projection = glm::ortho(0.0f, (float)WINDOW_WIDTH, 0.0f, (float)WINDOW_HEIGHT);
    glUniformMatrix4fv(glGetUniformLocation(shader, "projection"), 1, false, reinterpret_cast<float*>(&projection));
    glUniform3f(glGetUniformLocation(shader, "textColor"),
        this->options.color.x, this->options.color.y, this->options.color.z);
    glActiveTexture(GL_TEXTURE0);
    glBindVertexArray(VAO);

    // iterate through all characters
    for (const char& c : this->text)
    {
        font::Character ch = this->fonts->loadChar(c, this->options.font, this->options.font_size);

        float xpos = x + ch.bearing.x * this->options.scale;
        float ypos = y - (ch.size.y - ch.bearing.y) * this->options.scale;

        float w = ch.size.x * this->options.scale;
        float h = ch.size.y * this->options.scale;
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
        x += (ch.advance >> 6) * this->options.scale; // bitshift by 6 to get value in pixels (2^6 = 64)
    }
    glBindVertexArray(0);
    glBindTexture(GL_TEXTURE_2D, 0);

    glUseProgram(0);
}

}