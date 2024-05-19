#include "client/gui/img/img.hpp"

#include <string>

namespace gui::img {

std::string getImgFilepath(ImgID img) {
    auto img_root = getRepoRoot() / "assets/imgs";
    switch (img) {
        default:
        case ImgID::Yoshi: return (img_root / "Yoshi.png").string();
        case ImgID::AwesomeSauce: return (img_root / "awesomeface.png").string();
        case ImgID::ItemFrame: return (img_root / "frame.png").string();
        case ImgID::SelectedFrame: return (img_root / "selected_frame.png").string();
        case ImgID::HealthPotion: return (img_root / "pot_health.png").string();
        case ImgID::UnknownPotion: return (img_root / "pot_unknown.png").string();
        case ImgID::InvisPotion: return (img_root / "pot_invisibility.png").string();
        case ImgID::FireSpell: return (img_root / "fire_wand.png").string();
        case ImgID::HealSpell: return (img_root / "heal_wand.png").string();
        case ImgID::Orb: return (img_root / "orb.png").string();
        case ImgID::Scroll: return (img_root / "scroll.png").string();
    }
}

}
