#pragma once

#include "shared/utilities/root_path.hpp"

#include <string>

namespace gui::img {

enum class ImgID {
    Yoshi
};

struct Img {
    unsigned int texture_id;
    std::size_t width;
    std::size_t height;
};

std::string getImgFilepath(ImgID img);

}