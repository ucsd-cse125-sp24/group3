#pragma once

#include "client/core.hpp"

#include <cstddef>
#include <string>
#include <unordered_map>

namespace gui::font {


/**
 * Abstract representation of the different fonts to use in our game
 */
enum class Font {
    MENU,
    TEXT,
    TITLE,
};

#define ALL_FONTS() { \
    Font::MENU, Font::TEXT, Font::TITLE \
}

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
    GREEN,
    BLUE,
    GRAY,
    WHITE,
    YELLOW,
};

/**
 * Mapping from preset font colors to RGB values
 */
glm::vec3 getRGB(Color color);

const int UNIT_LARGE_SIZE_PX = 128; // how many pixels a small font is on the unit screen size
enum class Size {
    SMALL,
    SMALLMEDIUM,
    MEDIUM,
    LARGE,
    XLARGE
};
const std::unordered_map<Size, float> SIZE_TO_SCALE = {
    {Size::SMALL,       0.25f},
    {Size::SMALLMEDIUM, 0.35f},
    {Size::MEDIUM,      0.50f},
    {Size::LARGE,       1.0f},
    {Size::XLARGE,      2.0f},
};

int getFontSizePx(Size size);
float getScaleFactor(Size size);

/**
 * Takes an amount of pixels, and scales it to be from a 1500x1000 screen to the current screen width/height
 * 
 * @param pixels Pixels on a 1500x1000 window
 * @returns corresponding amount of pixels on the current window
 */
float getRelativePixels(float pixels);


}
