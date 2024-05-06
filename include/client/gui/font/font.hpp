#pragma once

#include "client/core.hpp"

#include <cstddef>
#include <string>
#include <unordered_map>

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
 * Mappings from our specified abstract fonts to the file to load
 */
std::string getFilepath(Font font);

/**
 * Preset colors for text
 */
enum class Color {
    BLACK,
    RED,
    BLUE,
    GRAY
};

/**
 * Mapping from preset font colors to RGB values
 */
glm::vec3 getRGB(Color color);

const int UNIT_LARGE_SIZE_PX = 128; // how many pixels a small font is on the unit screen size
enum class Size {
    SMALL,
    MEDIUM,
    LARGE,
    XLARGE
};
const std::unordered_map<Size, float> SIZE_TO_SCALE = {
    {Size::SMALL,   0.25f},
    {Size::MEDIUM,  0.50f},
    {Size::LARGE,   1.0f},
    {Size::XLARGE,  2.0f},
};

float getFontSizePx(Size size);
float getScaleFactor(Size size);


}
