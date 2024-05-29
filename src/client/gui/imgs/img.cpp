#include "client/gui/img/img.hpp"
#include "shared/game/celltype.hpp"

#include <string>
#include <stack>

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
        case ImgID::MazeEmpty: return (img_root / "maze" / "empty.png").string();
        case ImgID::MazeWall: return (img_root / "maze" / "wall.png").string();
        case ImgID::MazeBlank: return (img_root / "maze" / "blank.png").string();
        case ImgID::MazeOrb: return (img_root / "maze" / "orb.png").string();
        case ImgID::MazeExit: return (img_root / "maze" / "exit.png").string();
        case ImgID::MazePlayer1: return (img_root / "maze" / "player1.png").string();
        case ImgID::MazePlayer2: return (img_root / "maze" / "player2.png").string();
        case ImgID::MazePlayer3: return (img_root / "maze" / "player3.png").string();
        case ImgID::MazePlayer4: return (img_root / "maze" / "player4.png").string();
    }
}

ImgID cellTypeToImage(CellType type) {
    switch (type) {
        case CellType::Empty:
            return ImgID::MazeEmpty;
        case CellType::Wall:
        case CellType::Pillar:
        case CellType::TorchDown:
        case CellType::TorchLeft:
        case CellType::TorchRight:
        case CellType::TorchUp:
            return ImgID::MazeWall;
        case CellType::Orb:
            return ImgID::MazeOrb;
        case CellType::Exit:
            return ImgID::MazeExit;
        case CellType::OutsideTheMaze:
            return ImgID::MazeBlank;
        default:
            return ImgID::MazeEmpty;
    }
}

}
