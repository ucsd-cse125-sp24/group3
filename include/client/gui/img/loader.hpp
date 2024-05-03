#pragma once

#include "client/gui/img/img.hpp"

#include <unordered_map>

namespace gui::img {

/**
 * This class is supposed to load images.
 * 
 * Unfortunately it doesn't work at all! Yipeeee!
 */
class Loader {
public:
    Loader() = default;

    bool init();

    const Img& getImg(ImgID img_id) const;

private:
    std::unordered_map<ImgID, Img> img_map; 

    bool _loadImg(ImgID id);
};

}
