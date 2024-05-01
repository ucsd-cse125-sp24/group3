#pragma once

#include "client/gui/img/img.hpp"

#include <unordered_map>

namespace gui::img {

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
