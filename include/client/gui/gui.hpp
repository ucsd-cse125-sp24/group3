#pragma once

// #include "client/core.hpp"

// Include all gui headers so everyone else just needs to include this file
#include "client/gui/widget/options.hpp"
#include "client/gui/widget/type.hpp"
#include "client/gui/widget/widget.hpp"
#include "client/gui/widget/dyntext.hpp"
#include "client/gui/font/font.hpp"
#include "client/gui/font/loader.hpp"

#include <memory>

namespace gui {

class GUI {
public:
    GUI();

    bool init(GLuint text_shader);

    void render();

private:
    std::vector<widget::DynText> text;
    GLuint text_shader;

    std::shared_ptr<font::Loader> fonts;
};

}