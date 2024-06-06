#pragma once

#include "shared/utilities/root_path.hpp"
#include "client/core.hpp"

#include <initializer_list>
#include <string>

namespace gui::img {

/**
 * Abstract representation of any image we would want to load in
 * 
 * NOTE: if you add a new ID, then also add it into the below macro so that
 * the image loader will actually load it.
 */
enum class ImgID {
    Yoshi,
    AwesomeSauce,
    HealthPotion,
    UnknownPotion,
    InvisPotion,
    FireSpell,
    HealSpell,
    Orb,
    Mirror,
    Crosshair,
    Scroll,
    Dagger,
    Sword,
    Hammer,
    Title,
    LeftHotbar,
    RightHotbar,
    MiddleHotbar,
    MiddleSelected,
    DMLeftHotbar,
    DMRightHotbar,
    DMMiddleHotbar,
    DMMiddleSelected,
    DMMiddleCooldown,
    HealthBar,
    HealthTickEmpty,
    HealthTickFull,
    ManaBar,
    ManaTickEmpty,
    ManaTickFull,
    Needle,
    ItemBG,
    DMTrapBG,
    EventBG,
    DMEventBG,
    Compass0, Compass30, Compass60,
    Compass90, Compass120, Compass150,
    Compass180, Compass210, Compass240,
    Compass270, Compass300, Compass330,
    FloorSpikeTrap,
    Sungod,
    Teleporter,
    Lightning,
    ArrowTrap,
    SpikeTrap,
    DMCD_10, DMCD_9, DMCD_8, DMCD_7, DMCD_6, DMCD_5, DMCD_4, DMCD_3, DMCD_2, DMCD_1, 
    DMCD_Selected_10, DMCD_Selected_9, DMCD_Selected_8, DMCD_Selected_7, DMCD_Selected_6, 
    DMCD_Selected_5, DMCD_Selected_4, DMCD_Selected_3, DMCD_Selected_2, DMCD_Selected_1,
    Skull, 
    DestroyedSkull,
    SkullBG,
    HelpBG,
    HelpDMBG,
    ExitBG,
    ExitDMBG,
    ExitBGSelected,
    ExitDMBGSelected,
    ExitSelected,
    Exit,
    LobbyButton,
    Victory,
    Defeat,
    Death,
    Respawn,
    Blank,
};

// Sorry for whoever has to look at this :) - ted
#define GET_ALL_IMG_IDS() \
    {ImgID::Yoshi, ImgID::AwesomeSauce, ImgID::HealthPotion, ImgID::UnknownPotion, \
        ImgID::InvisPotion, ImgID::FireSpell, ImgID::HealSpell, ImgID::Orb, ImgID::Mirror, \
        ImgID::Crosshair, ImgID::Scroll, ImgID::Dagger, ImgID::Sword, ImgID::Hammer, \
        ImgID::LeftHotbar, ImgID::RightHotbar, ImgID::MiddleHotbar, ImgID::Blank, ImgID::Title, \
        ImgID::MiddleSelected, ImgID::HealthBar, ImgID::HealthTickEmpty, ImgID::HealthTickFull, \
        ImgID::ManaBar, ImgID::ManaTickEmpty, ImgID::ManaTickFull, ImgID::ItemBG, \
        ImgID::DMTrapBG, ImgID::Needle, ImgID::EventBG, ImgID::DMEventBG, \
        ImgID::Compass0, ImgID::Compass30, ImgID::Compass60, ImgID::Compass90, \
        ImgID::Compass120, ImgID::Compass150, ImgID::Compass180, ImgID::Compass210, \
        ImgID::Compass240, ImgID::Compass270, ImgID::Compass300, ImgID::Compass330, \
        ImgID::FloorSpikeTrap, ImgID::Sungod, ImgID::Teleporter, ImgID::Lightning, \
        ImgID::ArrowTrap, ImgID::SpikeTrap, \
        ImgID::DMTrapBG, ImgID::Needle, \
        ImgID::EventBG, ImgID::DMEventBG, \
        ImgID::DMLeftHotbar, ImgID::DMRightHotbar, ImgID::DMMiddleHotbar, ImgID::DMMiddleSelected, ImgID::DMMiddleCooldown, \
        ImgID::DMCD_10, ImgID::DMCD_9, ImgID::DMCD_8, ImgID::DMCD_7, ImgID::DMCD_6, \
        ImgID::DMCD_5, ImgID::DMCD_4, ImgID::DMCD_3, ImgID::DMCD_2, ImgID::DMCD_1, \
        ImgID::DMCD_Selected_10, ImgID::DMCD_Selected_9, ImgID::DMCD_Selected_8, ImgID::DMCD_Selected_7, ImgID::DMCD_Selected_6, \
        ImgID::DMCD_Selected_5, ImgID::DMCD_Selected_4, ImgID::DMCD_Selected_3, ImgID::DMCD_Selected_2, ImgID::DMCD_Selected_1, \
        ImgID::Skull, ImgID::DestroyedSkull, ImgID::SkullBG, ImgID::HelpBG, ImgID::HelpDMBG, \
        ImgID::ExitBG, ImgID::ExitDMBG, ImgID::LobbyButton, ImgID::Victory, ImgID::Defeat, \
        ImgID::ExitBGSelected, ImgID::ExitDMBGSelected, ImgID::ExitSelected, ImgID::Exit, \
        ImgID::Respawn, ImgID::Death, \
    }

/**
 * Representation of a loaded image
 */
struct Img {
    GLuint texture_id; /// opengl texture id
    int width;         /// width in pixels
    int height;        /// height in pixels
};

/**
 * Mapping from abstract image id to the filepath for that image
 * 
 * @param img ID of the image to get the filepath of
 */
std::string getImgFilepath(ImgID img);

}