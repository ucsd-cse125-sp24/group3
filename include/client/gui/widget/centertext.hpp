#pragma once

#include "client/gui/widget/flexbox.hpp"
#include "client/gui/widget/dyntext.hpp"

namespace gui::widget {

class CenterText {
public:
    static Flexbox::Ptr make(
        std::string text, 
        font::Font font, 
        font::FontSizePx size,
        font::FontColor color, 
        std::shared_ptr<font::Loader> fonts, 
        float y_pos
    );
};

}
