#pragma once

#include <cstddef>
#include <string>

namespace gui {

enum class Font {
    READABLE,
};

std::string getFontPath(Font font) {
    switch (font) {

    }
}

enum class FontSize {
    TINY,
    SMALL,
    MEDIUM,
    LARGE,
    HUGE
};

constexpr std::size_t getFontSizePt(FontSize size) {
    switch (size) {
        case FontSize::TINY: return 8;
        case FontSize::SMALL: return 12;
        default:
        case FontSize::MEDIUM: return 16;
        case FontSize::LARGE: return 24;
        case FontSize::HUGE: return 36;
    }
}

}