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
    TEXT
};

/**
 * Preset sizes for fonts
 */
enum FontSizePx {
    SMALL = 64,
    MEDIUM = 96,
    LARGE = 128,
    XLARGE = 256,
};

/**
 * Preset colors for text
 */
enum class FontColor {
    BLACK,
    RED,
    BLUE,
    GRAY
};

/**
 * Mappings from our specified abstract fonts to the file to load
 */
std::string getFilepath(Font font);

/**
 * Mapping from preset font colors to RGB values
 */
glm::vec3 getRGB(FontColor color);

}
