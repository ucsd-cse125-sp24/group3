#pragma once

// #include "client/core.hpp"

// Include all gui headers so everyone else just needs to include this file
#include "client/gui/widget/options.hpp"
#include "client/gui/widget/type.hpp"
#include "client/gui/widget/widget.hpp"
#include "client/gui/widget/dyntext.hpp"
#include "client/gui/widget/flexbox.hpp"
#include "client/gui/font/font.hpp"
#include "client/gui/font/loader.hpp"

#include <memory>
#include <unordered_map>

namespace gui {

using WidgetHandle = std::size_t;

class GUI {
public:

    GUI();

    bool init(GLuint text_shader);

    void render();

    WidgetHandle addWidget(widget::Widget::Ptr&& widget, float x, float y);
    std::unique_ptr<widget::Widget> removeWidget(WidgetHandle handle);

    void handleClick(float x, float y);
    void handleHover(float x, float y);

private:
    WidgetHandle next_handle {0};
    std::unordered_map<WidgetHandle, widget::Widget::Ptr> widgets;
    GLuint text_shader;

    std::shared_ptr<font::Loader> fonts;
};

}