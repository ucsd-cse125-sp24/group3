#include "client/gui/img/loader.hpp"
#include "client/core.hpp"

#include <iostream>

#include "stb_image.h"

namespace gui::img {

bool Loader::init() {
    std::cout << "Loading images...\n"; 

    for (auto img_id : GET_ALL_IMG_IDS()) {
        if (!this->_loadImg(img_id)) {
            return false;
        }
    }

    std::cout << "Loaded images\n";
    return true;
}

// reference: https://www.reddit.com/r/opengl/comments/57d21g/displaying_an_image_with_stb/
bool Loader::_loadImg(ImgID img_id) {
    auto path = getImgFilepath(img_id);
    std::cout << "Loading " << path << "...\n";

    int width, height, channels;

    unsigned char* img_data = stbi_load(path.c_str(), &width, &height, &channels, 4);

    GLuint texture_id;
    if (img_data == 0 || width == 0 || height == 0) {
        std::cerr << "Error loading " << path << "! " << img_data <<
            ", " << width << ", " << height << "\n" << std::endl;
        return false;
    }

    glGenTextures(1, &texture_id);
    glBindTexture(GL_TEXTURE_2D, texture_id);

    //set up some vars for OpenGL texturizing
	GLenum image_format = GL_RGBA;
	GLint internal_format = GL_RGBA;
	GLint level = 0;
	//store the texture data for OpenGL use
	glTexImage2D(GL_TEXTURE_2D, level, internal_format, width, height,
		0, image_format, GL_UNSIGNED_BYTE, img_data);

    stbi_image_free(img_data);

    glBindTexture(GL_TEXTURE_2D, 0);

    this->img_map.insert({img_id, Img {
        .texture_id = texture_id,
        .width = width,
        .height = height
    }});
    return true;
}

const Img& Loader::getImg(ImgID img_id) const {
    return this->img_map.at(img_id);
}

}
