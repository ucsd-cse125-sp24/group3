#include "client/gui/gui.hpp"

#include <memory>
#include <iostream>
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

    this->text_shader = text_shader;

    auto title = std::make_unique<widget::DynText>("Arcana", this->fonts);
    title->addOnClick([](){std::cout << "Clickie click\n";});

    this->addWidget(std::move(title), 0.0f, 0.0f);

    std::cout << "Initialized GUI\n";
    return true;
}

void GUI::render() {
    // for text rendering
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);  
    glEnable(GL_CULL_FACE);
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

    for (auto& [handle, widget] : this->widgets) {
        const auto& [bottom_left, _] = this->bboxes.at(handle);
        widget->render(this->text_shader, bottom_left.x, bottom_left.y);
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
    // convert to gui coords, where (0,0) is bottome left
    y = WINDOW_HEIGHT - y;

    for (const auto& [handle, bbox] : this->bboxes) {
        const auto& [bottom_left, top_right] = bbox;
        if (x > bottom_left.x && x < top_right.x && y > bottom_left.y && y < top_right.y) {
            this->widgets.at(handle)->doClick();
        }
    }
}

void GUI::handleHover(float x, float y) {
    // convert to gui coords, where (0,0) is bottome left
    y = WINDOW_HEIGHT - y;

    for (const auto& [handle, bbox] : this->bboxes) {
        const auto& [bottom_left, top_right] = bbox;
        if (x > bottom_left.x && x < top_right.x && y > bottom_left.y && y < top_right.y) {
            this->widgets.at(handle)->doHover();
        }
    }
}

}