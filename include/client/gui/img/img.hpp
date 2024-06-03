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
    Blank,
};
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
        ImgID::DMLeftHotbar, ImgID::DMRightHotbar, ImgID::DMMiddleHotbar, ImgID::DMMiddleSelected, \
        ImgID::FloorSpikeTrap, ImgID::Sungod, ImgID::Teleporter, ImgID::Lightning, \
        ImgID::ArrowTrap, ImgID::SpikeTrap, \
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