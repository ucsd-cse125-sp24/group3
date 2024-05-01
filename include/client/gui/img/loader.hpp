#pragma once

#include "client/gui/img/img.hpp"

#include <unordered_map>

namespace gui::img {

class Loader {
public:
    Loader() = default;

    bool init();

private:
    std::unordered_map<ImgID, Img> img_map; 

    void _loadImg();
};

}
