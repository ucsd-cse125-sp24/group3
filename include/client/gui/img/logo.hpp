#pragma once

#include "client/gui/img/img.hpp"

namespace gui::img {

class Logo {
public:
    static const inline std::size_t NUM_FRAMES = 156;

    Logo() = default;
    bool init();

    Img getNextFrame();
private:
    bool _loadFrame(std::size_t index);

    std::vector<Img> frames;
    std::size_t curr_frame;
};

}
