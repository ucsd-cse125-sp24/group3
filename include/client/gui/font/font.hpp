#pragma once

#include "client/core.hpp"

#include <cstddef>
#include <string>

namespace gui::font {

/**
 * Abstract representation of the different fonts to use in our game
 * 
 * NOTE: currently I haven't found a good font for "Text", so both of these
 * map to the same font.
 */
enum class Font {
    MENU,
    TEXT,
};

enum FontSizePx {
    SMALL = 64,
    MEDIUM = 96,
    LARGE = 128,
    BIG_YOSHI = 256
};

enum class FontColor {
    BLACK,
    RED,
    BLUE,
    GRAY
};

std::string getFilepath(Font font);

glm::vec3 getRGB(FontColor color);

}
