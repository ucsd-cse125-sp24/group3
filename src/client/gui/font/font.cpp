#include "client/gui/font/font.hpp"

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

}
