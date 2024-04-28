#pragma once

#include "client/core.hpp"
#include "client/gui/font/font.hpp"

// freetype needs this extra include for whatever unholy reason
#include <ft2build.h>
#include FT_FREETYPE_H  

#include <unordered_map>

namespace gui::font {

// modified from https://learnopengl.com/In-Practice/Text-Rendering

struct Character {
    unsigned int texture_id; /// id handle for glyph texture
    glm::ivec2 size;         /// size of glyph
    glm::ivec2 bearing;      /// offset from baseline to left/top of glyph
    unsigned int advance;    /// offset to advance to next glyph
};

struct font_pair_hash {
    std::size_t operator()(const std::pair<Font, FontSizePx>& p) const;
};

class Loader {
public:
    Loader() = default;

    bool init();

private:
    FT_Library ft;

    bool _loadFont(Font font);

    std::unordered_map<
        std::pair<Font, FontSizePx>,
        std::unordered_map<char, Character>,
        font_pair_hash
    > font_map;
};

}
