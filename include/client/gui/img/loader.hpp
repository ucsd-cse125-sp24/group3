#pragma once

#include "client/gui/img/img.hpp"

#include <unordered_map>

namespace gui::img {

/**
 * This class loads in all of our images
 */
class Loader {
public:
    Loader() = default;

    bool init();

    const Img& getImg(ImgID img_id) const;

private:
    std::unordered_map<ImgID, Img> img_map; 

    bool _loadImg(ImgID img_id);
};

}
