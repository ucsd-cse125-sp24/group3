#include "client/gui/gui.hpp"

#include <memory>
#include <iostream>
#include "shared/utilities/rng.hpp"


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

    this->text_shader = text_shader;

    this->addWidget(std::make_unique<widget::DynText>("Arcana", this->fonts), 0.0f, 0.0f);

    std::cout << "Initialized GUI\n";
    return true;
}

void GUI::render() {
    // for text rendering
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);  
    glEnable(GL_CULL_FACE);
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);


    for (auto& [_handle, widget] : this->widgets) {
        widget->render(this->text_shader, 0.0f, 0.0f);
    }

    glDisable(GL_CULL_FACE);
    glDisable(GL_BLEND);
}

WidgetHandle GUI::addWidget(std::unique_ptr<widget::Widget> widget, float x, float y) {
    WidgetHandle handle = this->next_handle++;
    glm::vec2 bottom_left(x, y);
    const auto& [width, height] = widget->getSize();
    glm::vec2 top_right(x + width, y + height);
    this->widgets.insert({handle, std::move(widget)});
    this->bboxes.insert({handle, {bottom_left, top_right}});
    return handle;
}

std::unique_ptr<widget::Widget> GUI::removeWidget(WidgetHandle handle) {
    auto widget = std::move(this->widgets.at(handle));
    this->widgets.erase(handle);
    this->bboxes.erase(handle);
    return widget;
}

// TODO: reduce copied code between these two functions

void GUI::handleClick(float x, float y) {
    for (const auto& [handle, bbox] : this->bboxes) {
        const auto& [bottom_left, top_right] = bbox;
        if (x > bottom_left.x && x < top_right.x && y > bottom_left.y && y < top_right.y) {
            this->widgets.at(handle)->doClick();
        }
    }
}

void GUI::handleHover(float x, float y) {
    for (const auto& [handle, bbox] : this->bboxes) {
        const auto& [bottom_left, top_right] = bbox;
        if (x > bottom_left.x && x < top_right.x && y > bottom_left.y && y < top_right.y) {
            this->widgets.at(handle)->doHover();
        }
    }
}

}