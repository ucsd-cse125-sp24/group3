#include "client/gui/img/logo.hpp"
#include "stb_image.h"
#include <sstream>
#include <iostream>

namespace gui::img {

bool Logo::init() {
    std::cout << "Loading all of the logo frames...\n";
    for (int i = 0; i < NUM_FRAMES; i++) {
        if (!_loadFrame(i)) {
            return false;
        }
    }

    return true;
}

Img Logo::getNextFrame() {
    std::size_t index = this->curr_frame;
    this->curr_frame++;
    if (this->curr_frame >= NUM_FRAMES) {
        this->curr_frame = 0;
    }
    return this->frames.at(index);
}

bool Logo::_loadFrame(std::size_t index) {
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

    std::stringstream ss;
    ss << "frame_" << index + 1 << ".png";

    auto path = getRepoRoot() / "assets/imgs/logo_animation" / ss.str();
    unsigned char* img_data = stbi_load(path.string().c_str(), &width, &height, &channels, 0);

    if (stbi_failure_reason()) {
        std::cout << "failure: " << stbi_failure_reason() << std::endl;
        return false;
    }

    if (img_data == 0 || width == 0 || height == 0) {
        std::cerr << "Error loading " << path << std::endl;
        return false;
    }

    
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, img_data);
    glGenerateMipmap(GL_TEXTURE_2D);

    // unbind texture
    glBindTexture(GL_TEXTURE_2D, 0);

    this->frames.push_back(Img {
        .texture_id = texture_id,
        .width = width,
        .height = height
    });

    stbi_image_free(img_data);

    return true;
}

};
