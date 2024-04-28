#include "client/gui/font/font.hpp"

#include "client/core.hpp"
#include "shared/utilities/root_path.hpp"

namespace gui::font {

std::string getFilepath(Font font) {
    auto dir = getRepoRoot() / "fonts";
    switch (font) {
        case Font::MENU: return (dir / "Lato-Regular.ttf").string();
        default:
        case Font::TEXT: return (dir / "Lato-Regular.ttf").string();
    } 
}

glm::vec3 getRGB(FontColor color) {
    switch (color) {
        case FontColor::RED:
            return {1.0f, 0.0f, 0.0f};
        case FontColor::BLUE:
            return {0.0f, 0.0f, 1.0f};
        default:
        case FontColor::BLACK:
            return {1.0f, 1.0f, 1.0f};
    }
}

}
