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
        case ImgID::Mirror: return (img_root / "mirror.png").string();
        case ImgID::Scroll: return (img_root / "scroll.png").string();
        case ImgID::Crosshair: return (img_root / "crosshair046.png").string();
        case ImgID::Dagger: return (img_root / "weapon_dagger.png").string();
        case ImgID::Sword: return (img_root / "weapon_sword.png").string();
        case ImgID::Hammer: return (img_root / "weapon_hammer.png").string();
        case ImgID::LeftHotbar: return (img_root / "left.png").string();
        case ImgID::RightHotbar: return (img_root / "right.png").string();
        case ImgID::MiddleHotbar: return (img_root / "middle.png").string();
        case ImgID::MiddleSelected: return (img_root / "selected_middle.png").string();
        case ImgID::DMLeftHotbar: return (img_root / "dm_left.png").string();
        case ImgID::DMRightHotbar: return (img_root / "dm_right.png").string();
        case ImgID::DMMiddleHotbar: return (img_root / "dm_middle.png").string();
        case ImgID::DMMiddleSelected: return (img_root / "dm_selected.png").string();
        case ImgID::DMMiddleCooldown: return (img_root / "dm_cooldown.png").string();
        case ImgID::Blank: return (img_root / "blank.png").string();
        case ImgID::HealthBar: return (img_root / "healthbar.png").string();
        case ImgID::HealthTickEmpty: return (img_root / "healthtick_empty.png").string();
        case ImgID::HealthTickFull: return (img_root / "healthtick_full.png").string();
        case ImgID::ManaBar: return (img_root / "manabar.png").string();
        case ImgID::ManaTickEmpty: return (img_root / "manatick_empty.png").string();
        case ImgID::ManaTickFull: return (img_root / "manatick_full.png").string();
        case ImgID::Needle: return (img_root / "needle.png").string();
        case ImgID::ItemBG: return (img_root / "itemBackground.png").string();
        case ImgID::DMTrapBG: return (img_root / "dmBackground.png").string();
        case ImgID::EventBG: return (img_root / "eventBG.png").string();
        case ImgID::DMEventBG: return (img_root / "dmEventBG.png").string();
        case ImgID::Compass0: return (img_root / "compasses/compass_0.png").string();
        case ImgID::Compass30: return (img_root / "compasses/compass_30.png").string();
        case ImgID::Compass60: return (img_root / "compasses/compass_60.png").string();
        case ImgID::Compass90: return (img_root / "compasses/compass_90.png").string();
        case ImgID::Compass120: return (img_root / "compasses/compass_120.png").string();
        case ImgID::Compass150: return (img_root / "compasses/compass_150.png").string();
        case ImgID::Compass180: return (img_root / "compasses/compass_180.png").string();
        case ImgID::Compass210: return (img_root / "compasses/compass_210.png").string();
        case ImgID::Compass240: return (img_root / "compasses/compass_240.png").string();
        case ImgID::Compass270: return (img_root / "compasses/compass_270.png").string();
        case ImgID::DMCD_10: return (img_root / "dm_cooldown/dm_cooldown_10.png").string();
        case ImgID::DMCD_9: return (img_root / "dm_cooldown/dm_cooldown_9.png").string();
        case ImgID::DMCD_8: return (img_root / "dm_cooldown/dm_cooldown_8.png").string();
        case ImgID::DMCD_7: return (img_root / "dm_cooldown/dm_cooldown_7.png").string();
        case ImgID::DMCD_6: return (img_root / "dm_cooldown/dm_cooldown_6.png").string();
        case ImgID::DMCD_5: return (img_root / "dm_cooldown/dm_cooldown_5.png").string();
        case ImgID::DMCD_4: return (img_root / "dm_cooldown/dm_cooldown_4.png").string();
        case ImgID::DMCD_3: return (img_root / "dm_cooldown/dm_cooldown_3.png").string();
        case ImgID::DMCD_2: return (img_root / "dm_cooldown/dm_cooldown_2.png").string();
        case ImgID::DMCD_1: return (img_root / "dm_cooldown/dm_cooldown_1.png").string();
        case ImgID::DMCD_Selected_10: return (img_root / "dm_cooldown/dm_selected_cooldown_10.png").string();
        case ImgID::DMCD_Selected_9: return (img_root / "dm_cooldown/dm_selected_cooldown_9.png").string();
        case ImgID::DMCD_Selected_8: return (img_root / "dm_cooldown/dm_selected_cooldown_8.png").string();
        case ImgID::DMCD_Selected_7: return (img_root / "dm_cooldown/dm_selected_cooldown_7.png").string();
        case ImgID::DMCD_Selected_6: return (img_root / "dm_cooldown/dm_selected_cooldown_6.png").string();
        case ImgID::DMCD_Selected_5: return (img_root / "dm_cooldown/dm_selected_cooldown_5.png").string();
        case ImgID::DMCD_Selected_4: return (img_root / "dm_cooldown/dm_selected_cooldown_4.png").string();
        case ImgID::DMCD_Selected_3: return (img_root / "dm_cooldown/dm_selected_cooldown_3.png").string();
        case ImgID::DMCD_Selected_2: return (img_root / "dm_cooldown/dm_selected_cooldown_2.png").string();
        case ImgID::DMCD_Selected_1: return (img_root / "dm_cooldown/dm_selected_cooldown_1.png").string();
        case ImgID::Compass300: return (img_root / "compasses/compass_300.png").string();
        case ImgID::Compass330: return (img_root / "compasses/compass_330.png").string();
        case ImgID::Sungod: return (img_root / "sungod.png").string();
        case ImgID::Lightning: return (img_root / "lightning.png").string();
        case ImgID::LightCut: return (img_root / "lightcut.png").string();
        case ImgID::Teleporter: return (img_root / "teleporter.png").string();
        case ImgID::FloorSpikeTrap: return (img_root / "floorspiketrap.png").string();
        case ImgID::ArrowTrap: return (img_root / "arrowtrap.png").string();
        case ImgID::SpikeTrap: return (img_root / "spiketrap.png").string();
        case ImgID::Skull: return (img_root / "normalSkull.png").string();
        case ImgID::DestroyedSkull: return (img_root / "destroyedSkull.png").string();
        case ImgID::SkullBG: return (img_root / "deathCountBG.png").string();
        case ImgID::HelpBG: return (img_root / "helpBG.png").string();
        case ImgID::HelpDMBG: return (img_root / "helpDMBG.png").string();
        case ImgID::ExitBG: return (img_root / "exitBG.png").string();
        case ImgID::ExitDMBG: return (img_root / "exitDMBG.png").string();
        case ImgID::ExitBGSelected: return (img_root / "exitBGSelected.png").string();
        case ImgID::ExitDMBGSelected: return (img_root / "exitDMBGSelected.png").string();
        case ImgID::Exit: return (img_root / "exit.png").string();
        case ImgID::ExitSelected: return (img_root / "exitSelected.png").string();
        case ImgID::LobbyButton: return (img_root / "lobbyButton.png").string();
        case ImgID::Victory: return (img_root / "victory.png").string();
        case ImgID::Defeat: return (img_root / "defeat.png").string();
        case ImgID::Death: return (img_root / "died.png").string();
        case ImgID::Respawn: return (img_root / "respawn.png").string();
        case ImgID::StartGame: return (img_root / "start.png").string();
        case ImgID::StartGameSelected: return (img_root / "startHover.png").string();
        case ImgID::RowBG: return (img_root / "rowBG.png").string();
        case ImgID::Player: return (img_root / "player.png").string();
        case ImgID::PlayerSelected: return (img_root / "playerSelected.png").string();
        case ImgID::Zeus: return (img_root / "zeus.png").string();
        case ImgID::ZeusSelected: return (img_root / "zeusSelected.png").string();
        case ImgID::ReadyPlayer: return (img_root / "readyPlayer.png").string();
        case ImgID::ReadyPlayerSelected: return (img_root / "readyPlayerSelected.png").string();
        case ImgID::ReadyZeus: return (img_root / "readyZeus.png").string();
        case ImgID::ReadyZeusSelected: return (img_root / "readyZeusSelected.png").string();
    }
}
}
