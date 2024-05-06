#include "client/gui/font/font.hpp"

#include "client/core.hpp"
#include "client/constants.hpp"
#include "client/client.hpp"
#include "shared/utilities/root_path.hpp"

namespace gui::font {

float getFontSizePx(Size size) {
    return UNIT_LARGE_SIZE_PX * getScaleFactor(size);
}

float getScaleFactor(Size size) {
    float screen_factor = WINDOW_WIDTH / UNIT_WINDOW_WIDTH;

    std::cout << SIZE_TO_SCALE.at(size) * screen_factor << "\n";
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
