#include "client/gui/widget/flexbox.hpp"

#include <vector>
#include <memory>
#include <iostream>

#include "client/core.hpp"

namespace gui::widget {

Flexbox::Flexbox(glm::vec2 origin, glm::vec2 size, Flexbox::Options options):
    Widget(Type::Flexbox, origin), options(options)
{
    this->width = size.x;
    this->height = size.y;
}

Flexbox::Flexbox(glm::vec2 origin, Flexbox::Options options):
    Flexbox(origin, {0.0f, 0.0f}, options) {}

Flexbox::Flexbox(glm::vec2 origin, glm::vec2 size):
    Flexbox(origin, size, Flexbox::Options { 
        .direction = JustifyContent::HORIZONTAL,
        .alignment = AlignItems::LEFT,
        .padding   = 0
    }) {}

Flexbox::Flexbox(glm::vec2 origin):
    Flexbox(origin, {0.0f, 0.0f}) {}

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

    // Bless this mess!

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
        prev_height = prev_widget->getSize().second;
    }

    if (this->options.direction == JustifyContent::HORIZONTAL) {
        this->width += new_width + this->options.padding;
        this->height = std::max(this->height, new_height);
        glm::vec2 new_origin(prev_origin.x + prev_width + this->options.padding, prev_origin.y);
        widget->setOrigin(new_origin);
    } else if (this->options.direction == JustifyContent::VERTICAL) {
        this->height += new_height + this->options.padding;
        this->width = std::max(this->width, new_width);
        glm::vec2 new_origin(prev_origin.x, prev_origin.y + prev_height + this->options.padding);
        widget->setOrigin(new_origin);
    }

    this->widgets.push_back(std::move(widget));

    if (this->options.alignment == AlignItems::CENTER) {
        if (this->options.direction == JustifyContent::HORIZONTAL) {
            std::cerr << "Note: center alignment not yet implemented for horizontal justify. Doing nothing\n";
        } else if (this->options.direction == JustifyContent::VERTICAL) {
            for (auto& widget : this->widgets) {
                const auto [curr_width, _] = widget->getSize();
                glm::vec2 new_origin(this->origin.x + ((this->width - curr_width) / 2.0f), widget->getOrigin().y);
                widget->setOrigin(new_origin);
            }
        }
    } else if (this->options.alignment == AlignItems::RIGHT) {
        std::cerr << "Note: right alignment not yet implemented. Doing nothing\n";
    } // else it is align left, which is default behavior and requires no more messing

}

void Flexbox::render(GLuint shader) {
    // use x and y as origin coordinates, and render everything else based off of it

    for (const auto& widget : this->widgets) {
        widget->render(shader);
    }
}


}

