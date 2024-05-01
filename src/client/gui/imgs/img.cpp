#include "client/gui/img/img.hpp"

#include <string>

namespace gui::img {

std::string getImgFilepath(ImgID img) {
    auto img_root = getRepoRoot() / "imgs";
    switch (img) {
        default:
        case ImgID::Yoshi: return (img_root / "Yoshi.png").string();
    }
}

}
