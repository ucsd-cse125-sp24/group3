#pragma once

// #include "client/core.hpp"

// Include all gui headers so everyone else just needs to include this file
#include "client/gui/widget/options.hpp"
#include "client/gui/widget/type.hpp"
#include "client/gui/widget/widget.hpp"
#include "client/gui/font/font.hpp"
#include "client/gui/font/loader.hpp"

namespace gui {

class GUI {
public:
    GUI() = default;

    bool init();

private:
    font::Loader fonts;
};

}