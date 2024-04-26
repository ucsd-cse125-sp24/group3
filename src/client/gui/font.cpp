#include "client/gui/font.hpp"

namespace gui {

std::string getFontPath(Font font) {
    switch (font) {
        case Font::READABLE: return "/path/to/readable/font";
    }
}

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
