#include "client/gui/gui.hpp"

#include <memory>
#include <iostream>
#include <vector>
#include "shared/utilities/rng.hpp"
#include "client/client.hpp"


namespace gui {

GUI::GUI() {

}

bool GUI::init(GLuint text_shader)
{
    std::cout << "Initializing GUI...\n";

    this->fonts = std::make_shared<font::Loader>();

    if (!this->fonts->init()) {
        return false;
    }

    if (!this->images.init()) {
        return false;
    }

    this->text_shader = text_shader;

    std::cout << "Initialized GUI\n";
    return true;
}

void GUI::beginFrame() {
    std::unordered_map<widget::Handle, widget::Widget::Ptr> empty;
    std::swap(this->widgets, empty);
}


void GUI::renderFrame() {
    // for text rendering
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);  
    glEnable(GL_CULL_FACE);
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

    for (auto& [handle, widget] : this->widgets) {
        widget->render(this->text_shader);
    }

    glDisable(GL_CULL_FACE);
    glDisable(GL_BLEND);
}

void GUI::endFrame() {
}

widget::Handle GUI::addWidget(widget::Widget::Ptr&& widget) {
    widget::Handle handle = this->next_handle++;
    this->widgets.insert({handle, std::move(widget)});
    return handle;
}

std::unique_ptr<widget::Widget> GUI::removeWidget(widget::Handle handle) {
    auto widget = std::move(this->widgets.at(handle));
    this->widgets.erase(handle);
    return widget;
}

// TODO: reduce copied code between these two functions

void GUI::handleClick(float x, float y) {
    // convert to gui coords, where (0,0) is bottome left
    y = WINDOW_HEIGHT - y;

    for (const auto& [_, widget] : this->widgets) {
        widget->doClick(x, y);
    }
}

void GUI::handleHover(float x, float y) {
    // convert to gui coords, where (0,0) is bottome left
    y = WINDOW_HEIGHT - y;

    for (const auto& [_, widget] : this->widgets) {
        widget->doHover(x, y);
    }
}

std::shared_ptr<font::Loader> GUI::getFonts() {
    return this->fonts;
}

}
