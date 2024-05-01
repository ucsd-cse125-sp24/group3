#pragma once

#include "shared/utilities/root_path.hpp"
#include "client/core.hpp"

#include <initializer_list>
#include <string>

namespace gui::img {

enum class ImgID {
    Yoshi
};

#define GET_ALL_IMG_IDS() \
    {ImgID::Yoshi}

struct Img {
    GLuint texture_id;
    int width;
    int height;
};

std::string getImgFilepath(ImgID img);

}