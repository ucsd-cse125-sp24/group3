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
    Compass,
    ItemBG,
    DMTrapBG,
    Compass0,
    Compass90,
    Compass180,
    Compass270,
    Blank,
};
#define GET_ALL_IMG_IDS() \
    {ImgID::Yoshi, ImgID::AwesomeSauce, ImgID::HealthPotion, ImgID::UnknownPotion, \
        ImgID::InvisPotion, ImgID::FireSpell, ImgID::HealSpell, ImgID::Orb, \
        ImgID::Crosshair, ImgID::Scroll, ImgID::Dagger, ImgID::Sword, ImgID::Hammer, \
        ImgID::LeftHotbar, ImgID::RightHotbar, ImgID::MiddleHotbar, ImgID::Blank, ImgID::Title, \
        ImgID::MiddleSelected, ImgID::HealthBar, ImgID::HealthTickEmpty, ImgID::HealthTickFull, \
        ImgID::ManaBar, ImgID::ManaTickEmpty, ImgID::ManaTickFull, ImgID::ItemBG, \
        ImgID::DMTrapBG, ImgID::Needle, ImgID::Compass, ImgID::Compass0, \
        ImgID::Compass90, ImgID::Compass180, ImgID::Compass270, \
        ImgID::DMLeftHotbar, ImgID::DMRightHotbar, ImgID::DMMiddleHotbar, ImgID::DMMiddleSelected, \
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