#pragma once

// #include "client/core.hpp"


// Include all gui headers so everyone else just needs to include this file
#include "client/gui/widget/options.hpp"
#include "client/gui/widget/type.hpp"
#include "client/gui/widget/widget.hpp"
#include "client/gui/widget/dyntext.hpp"
#include "client/gui/widget/flexbox.hpp"
#include "client/gui/widget/staticimg.hpp"
#include "client/gui/font/font.hpp"
#include "client/gui/font/loader.hpp"
#include "client/gui/img/img.hpp"
#include "client/gui/img/loader.hpp"

#include <memory>
#include <unordered_map>

namespace gui {

using WidgetHandle = std::size_t;

class GUI {
public:
    GUI();

    bool init(GLuint text_shader);

    void beginFrame();
    void renderFrame();
    void endFrame();

    WidgetHandle addWidget(widget::Widget::Ptr&& widget);
    std::unique_ptr<widget::Widget> removeWidget(WidgetHandle handle);

    void handleClick(float x, float y);
    void handleHover(float x, float y);

    void clearAll();

    std::shared_ptr<font::Loader> getFonts();

private:
    WidgetHandle next_handle {0};
    std::unordered_map<WidgetHandle, widget::Widget::Ptr> widgets;
    GLuint text_shader;

    std::shared_ptr<font::Loader> fonts;
    img::Loader images;
};

using namespace gui;

}