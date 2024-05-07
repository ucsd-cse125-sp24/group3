#include "client/gui/font/font.hpp"

#include "client/core.hpp"
#include "client/constants.hpp"
#include "client/client.hpp"
#include "shared/utilities/root_path.hpp"

namespace gui::font {

int getFontSizePx(Size size) {
    return UNIT_LARGE_SIZE_PX * getScaleFactor(size);
}

float getScaleFactor(Size size) {
    float screen_factor = static_cast<float>(WINDOW_WIDTH) / static_cast<float>(UNIT_WINDOW_WIDTH);

    return SIZE_TO_SCALE.at(size) * screen_factor;
}

std::string getFilepath(Font font) {
    auto dir = getRepoRoot() / "assets/fonts";
    switch (font) {
        case Font::MENU: return (dir / "AncientModernTales-a7Po.ttf").string();
        default:
        case Font::TEXT: return (dir / "AncientModernTales-a7Po.ttf").string();
    } 
}

glm::vec3 getRGB(Color color) {
    switch (color) {
        case Color::RED:
            return {1.0f, 0.0f, 0.0f};
        case Color::BLUE:
            return {0.0f, 0.0f, 1.0f};
        case Color::GRAY:
            return {0.5f, 0.5f, 0.5f};

        case Color::BLACK:
        default:
            return {0.0f, 0.0f, 0.0f};
    }
}

}