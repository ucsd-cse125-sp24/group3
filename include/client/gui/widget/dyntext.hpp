#pragma once

#include <string>

#include "client/gui/widget/widget.hpp"
#include "client/gui/font/font.hpp"

namespace gui::widget {

class DynText : public Widget {
public:
    struct Options {
        font::Font font {font::Font::TEXT};
        font::FontSizePx font_size {font::FontSizePx::MEDIUM};
    };

    DynText(std::string text, Options options = {});

    void render() override;

private:
    Options options;

};

}
