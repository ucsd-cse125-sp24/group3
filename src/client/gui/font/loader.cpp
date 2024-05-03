#include "client/gui/font/loader.hpp"

#include <iostream>

// freetype needs this extra include for whatever unholy reason
#include <ft2build.h>
#include FT_FREETYPE_H  

#include "shared/utilities/root_path.hpp"

namespace gui::font {

std::size_t font_pair_hash::operator()(const std::pair<Font, FontSizePx>& p) const {
    // idk if this is actually doing what I think it is doing
    return (static_cast<std::size_t>(p.first) << 32 ^ p.second);
}

bool Loader::init() {
    if (FT_Init_FreeType(&this->ft)) {
        std::cerr << "ERROR::FREETYPE: Could not init FreeType Library" << std::endl;
        return false;
    }

    // we mess with some alignment when creating the textures, 
    // so this is supposed to prevent seg faults related to that
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

    if (!this->_loadFont(Font::MENU)) {
        return false;
    }
    if (!this->_loadFont(Font::TEXT)) {
        return false;
    }

    FT_Done_FreeType(this->ft); // done loading fonts, so can release these resources

    return true;
}

const Character& Loader::loadChar(char c, Font font, FontSizePx size) const {
    if (!this->font_map.contains({font, size}) || !this->font_map.at({font, size}).contains(c)) {
        return this->font_map.at({Font::TEXT, FontSizePx::MEDIUM}).at('?');
    }

    return this->font_map.at({font, size}).at(c);
}

bool Loader::_loadFont(Font font) {
    auto path = font::getFilepath(font);

    std::cout << "Loading font: " << path << "\n";

    FT_Face face;
    if (FT_New_Face(this->ft, path.c_str(), 0, &face)) {
        std::cout << "ERROR::FREETYPE: Failed to load font at " << path << std::endl;  
        return false;
    }

    for (auto font_size : {FontSizePx::SMALL, FontSizePx::MEDIUM, FontSizePx::LARGE, FontSizePx::HUGE}) {
        FT_Set_Pixel_Sizes(face, 0, font_size);
        std::unordered_map<unsigned char, Character> characters;
        for (unsigned char c = 0; c < 128; c++) {
            // load character glyph 
            if (FT_Load_Char(face, c, FT_LOAD_RENDER)) {
                std::cerr << "ERROR::FREETYTPE: Failed to load Glyph " << c << std::endl;
                return false;
            }

            // generate texture
            unsigned int texture;
            glGenTextures(1, &texture);
            glBindTexture(GL_TEXTURE_2D, texture);
            glTexImage2D(
                GL_TEXTURE_2D,
                0,
                GL_RED,
                face->glyph->bitmap.width,
                face->glyph->bitmap.rows,
                0,
                GL_RED,
                GL_UNSIGNED_BYTE,
                face->glyph->bitmap.buffer
            );
            // set texture options
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            // now store character for later use
            Character character = {
                texture, 
                glm::ivec2(face->glyph->bitmap.width, face->glyph->bitmap.rows),
                glm::ivec2(face->glyph->bitmap_left, face->glyph->bitmap_top),
                static_cast<unsigned int>(face->glyph->advance.x)
            };
            characters.insert({c, character});
        }
  
        this->font_map.insert({{font, font_size}, characters});
    }

    FT_Done_Face(face);

    return true;
}

}
