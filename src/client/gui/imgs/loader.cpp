#include "client/gui/img/loader.hpp"
#include "client/core.hpp"

#include <iostream>
#include <algorithm>

#include "stb_image.h"

namespace gui::img {

bool Loader::init() {
    std::cout << "Loading images...\n"; 

    for (auto img_id : GET_ALL_IMG_IDS()) {
        if (!this->_loadImg(img_id)) { // cppcheck-suppress useStlAlgorithm
            return false;
        }
    }

    std::cout << "Loaded images\n";
    return true;
}

bool Loader::_loadImg(ImgID img_id) {
    GLuint texture_id;

    glGenTextures(1, &texture_id);
    glBindTexture(GL_TEXTURE_2D, texture_id);

    // set Texture wrap and filter modes
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    int width, height, channels;

	stbi_set_flip_vertically_on_load(true);

    auto path = getImgFilepath(img_id);
    std::cout << "Loading " << path << "...\n";
    unsigned char* img_data = stbi_load(path.c_str(), &width, &height, &channels, 0);

    if (stbi_failure_reason())
        std::cout << "failure: " << stbi_failure_reason() << std::endl;

    if (img_data == 0 || width == 0 || height == 0) {
        std::cerr << "Error loading " << path << std::endl;
        return false;
    }
    
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, img_data);
    glGenerateMipmap(GL_TEXTURE_2D);

    // unbind texture
    glBindTexture(GL_TEXTURE_2D, 0);

    this->img_map.insert({img_id, Img {
        .texture_id = texture_id,
        .width = width,
        .height = height
    }});

    stbi_image_free(img_data);

    return true;
}

const Img& Loader::getImg(ImgID img_id) const {
    return this->img_map.at(img_id);
}

}
