#pragma once

#include <string>

#include "client/gui/widget/widget.hpp"
#include "client/gui/font.hpp"

namespace gui {
namespace widget {

class DynText : public Widget {
public:
    struct Options {
        Font font {Font::READABLE};
        FontSize font_size {FontSize::MEDIUM};
    };

    DynText(std::string text, Options options = {});

    void render() override;

private:
    Options options;

};

}
}
