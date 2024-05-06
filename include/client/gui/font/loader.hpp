#pragma once

#include "client/core.hpp"
#include "client/gui/font/font.hpp"

// freetype needs this extra include for whatever unholy reason
#include <ft2build.h>
#include FT_FREETYPE_H  

#include <unordered_map>

namespace gui::font {

// modified from https://learnopengl.com/In-Practice/Text-Rendering

/**
 * Representation of a font character
 */
struct Character {
    unsigned int texture_id; /// id handle for glyph texture
    glm::ivec2 size;         /// size of glyph
    glm::ivec2 bearing;      /// offset from baseline to left/top of glyph
    unsigned int advance;    /// offset to advance to next glyph
};

/**
 * Handles loading all of the fonts we want to use, and provides an interface to get
 * the Character information (e.g. opengl texture and sizing information).
 */
class Loader {
public:
    Loader() = default;

    /**
     * Initializes all of the font information for every font we want to use and stores
     * it inside of the font map data member.
     */
    bool init();

    /**
     * Loads the specified character with the specified font and size
     * 
     * @param c ASCII char to load
     * @param font Abstract font to use.
     * @returns the Character information for that glyph
     */
    [[nodiscard]] const Character& loadChar(char c, Font font) const;

private:
    FT_Library ft;

    /**
     * Internal helper function to load a font. Called in `init()` 
     */
    bool _loadFont(Font font);

    std::unordered_map<
        std::pair<Font, Size>,
        std::unordered_map<unsigned char, Character>
    > font_map;
};

}
