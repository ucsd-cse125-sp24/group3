#include "client/gui/img/img.hpp"

#include <string>

namespace gui::img {

std::string getImgFilepath(ImgID img) {
    auto img_root = getRepoRoot() / "assets/imgs";
    switch (img) {
        default:
        case ImgID::Title: return (img_root / "title.png").string();
        case ImgID::Yoshi: return (img_root / "Yoshi.png").string();
        case ImgID::AwesomeSauce: return (img_root / "awesomeface.png").string();
        case ImgID::HealthPotion: return (img_root / "pot_health.png").string();
        case ImgID::UnknownPotion: return (img_root / "pot_unknown.png").string();
        case ImgID::InvisPotion: return (img_root / "pot_invisibility.png").string();
        case ImgID::FireSpell: return (img_root / "fire_wand.png").string();
        case ImgID::HealSpell: return (img_root / "heal_wand.png").string();
        case ImgID::Orb: return (img_root / "orb.png").string();
        case ImgID::Scroll: return (img_root / "scroll.png").string();
        case ImgID::Crosshair: return (img_root / "crosshair046.png").string();
        case ImgID::Dagger: return (img_root / "weapon_dagger.png").string();
        case ImgID::Sword: return (img_root / "weapon_sword.png").string();
        case ImgID::Hammer: return (img_root / "weapon_hammer.png").string();
        case ImgID::LeftHotbar: return (img_root / "left.png").string();
        case ImgID::RightHotbar: return (img_root / "right.png").string();
        case ImgID::MiddleHotbar: return (img_root / "middle.png").string();
        case ImgID::MiddleSelected: return (img_root / "selected_middle.png").string();
        case ImgID::Blank: return (img_root / "blank.png").string();
        case ImgID::HealthBar: return (img_root / "healthbar.png").string();
        case ImgID::HealthTickEmpty: return (img_root / "healthtick_empty.png").string();
        case ImgID::HealthTickFull: return (img_root / "healthtick_full.png").string();
        case ImgID::ManaBar: return (img_root / "manabar.png").string();
        case ImgID::ManaTickEmpty: return (img_root / "manatick_empty.png").string();
        case ImgID::ManaTickFull: return (img_root / "manatick_full.png").string();
    }
}

}
