#pragma once

#include "client/core.hpp"

#include <cstddef>
#include <string>

namespace gui::font {

enum class Font {
    MENU,
    TEXT,
};

enum FontSizePx {
    SMALL = 12,
    MEDIUM = 24,
    LARGE = 36
};

enum class FontColor {
    BLACK,
    RED,
    BLUE
};

std::string getFilepath(Font font);

glm::vec3 getRGB(FontColor color);

}
