#include "client/gui/widget/flexbox.hpp"

#include <vector>
#include <memory>

#include "client/core.hpp"

namespace gui::widget {

Flexbox::Ptr Flexbox::make(glm::vec2 origin, Flexbox::Options options) {
    return std::make_unique<Flexbox>(origin, options);
}

Flexbox::Ptr Flexbox::make(glm::vec2 origin) {
    return std::make_unique<Flexbox>(origin);
}

Flexbox::Flexbox(glm::vec2 origin, Flexbox::Options options):
    Widget(Type::Flexbox, origin),
    options(options) {}

Flexbox::Flexbox(glm::vec2 origin):
    Flexbox(origin, Flexbox::Options { .direction=JustifyContent::HORIZONTAL }) {}

void Flexbox::doClick(float x, float y) {
    Widget::doClick(x, y);
    for (auto& widget : this->widgets) {
        widget->doClick(x, y);
    }
}

void Flexbox::doHover(float x, float y) {
    Widget::doHover(x, y);
    for (auto& widget : this->widgets) {
        widget->doHover(x, y);
    }
}

void Flexbox::push(Widget::Ptr&& widget) {
    const auto& [new_width, new_height] = widget->getSize();

    glm::vec2 prev_origin;
    std::size_t prev_width;
    std::size_t prev_height;
    if (this->widgets.empty()) {
        prev_origin = this->origin;
        prev_width = this->width;
        prev_height = this->height;
    } else {
        Widget::Ptr& prev_widget = this->widgets[this->widgets.size() - 1];
        prev_origin = prev_widget->getOrigin();
        prev_width = prev_widget->getSize().first;
        prev_width = prev_widget->getSize().second;
    }

    if (this->options.direction == JustifyContent::HORIZONTAL) {
        this->width += new_width;
        this->height = std::max(this->height, new_height);
        glm::vec2 new_origin(prev_origin.x + prev_width, prev_origin.y);
        widget->setOrigin(new_origin);
    } else if (this->options.direction == JustifyContent::VERTICAL) {
        this->height += new_height;
        this->width = std::max(this->width, new_width);
        glm::vec2 new_origin(prev_origin.x, prev_origin.y + prev_height);
        widget->setOrigin(new_origin);
    }

    this->widgets.push_back(std::move(widget));
}

void Flexbox::render(GLuint shader) {
    // use x and y as origin coordinates, and render everything else based off of it

    for (const auto& widget : this->widgets) {
        widget->render(shader);
    }
}


}

