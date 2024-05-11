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
    ItemFrame,
    HealthPotion,
    SwiftnessPotion,
    InvisPotion
};
#define GET_ALL_IMG_IDS() \
    {ImgID::Yoshi, ImgID::AwesomeSauce, ImgID::ItemFrame, ImgID::HealthPotion, ImgID::SwiftnessPotion, ImgID::InvisPotion}

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