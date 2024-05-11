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
        prev_width = 0;
        prev_height = 0;
    } else {
        Widget::Ptr& prev_widget = this->widgets[this->widgets.size() - 1];
        prev_origin = prev_widget->getOrigin();
        prev_width = prev_widget->getSize().first;
        prev_height = prev_widget->getSize().second;
    }

    if (this->options.direction == Dir::HORIZONTAL) {
        float widgets_width = 0.0f;
        for (auto& widget : this->widgets) {
            widgets_width += widget->getSize().first;
        }
        if (widgets_width >= this->width) {
            this->width += new_width + this->options.padding;
        }
        this->height = std::max(this->height, new_height);
        glm::vec2 new_origin(prev_origin.x + prev_width + this->options.padding, prev_origin.y);
        widget->setOrigin(new_origin);
    } else if (this->options.direction == Dir::VERTICAL) {
        this->height += new_height + this->options.padding;
        this->width = std::max(this->width, new_width);
        glm::vec2 new_origin(prev_origin.x, prev_origin.y + prev_height + this->options.padding);
        widget->setOrigin(new_origin);
    }

    this->widgets.push_back(std::move(widget));

    if (this->options.alignment == Align::CENTER) {
        if (this->options.direction == Dir::HORIZONTAL) {
            // it is much easier to handle this case in the close function
        } else if (this->options.direction == Dir::VERTICAL) {
            for (auto& widget : this->widgets) {
                const auto [curr_width, _] = widget->getSize();
                glm::vec2 new_origin(this->origin.x + ((this->width - curr_width) / 2.0f), widget->getOrigin().y);
                widget->setOrigin(new_origin);
            }
        }
    } else if (this->options.alignment == Align::RIGHT) {
        std::cerr << "Note: right alignment not yet implemented. Doing nothing\n";
    } // else it is align left, which is default behavior and requires no more messing

}

void Flexbox::lock() {
    // It is easier to do this kind of alignment once everything is in the container,
    // so we do it now
    // dear god this code is so gross
    if (this->options.alignment == Align::CENTER) {
        if (this->options.direction == Dir::HORIZONTAL) {
            // need to shift everything left/right to center align
            // this->width should be set to a size larger than all of the elements
            // e.g     |               | <- marks this->width, so we are centering within this->width
            //         |               | * this comes about by giving a definite width when constructing the flex box,
            //         |               | * which is usually WINDOW_WIDTH since we want to center within the entire window
            // before: 1 2 3 4 5 . . . . 
            // after:  . . 1 2 3 4 5 . . 
            float widgets_width = 0.0f;
            for (auto& widget : this->widgets) {
                widgets_width += widget->getSize().first;
            }
            float shift_amount = (this->width - widgets_width) / 2.0f;
            for (auto& widget : this->widgets) {
                widget->setOrigin(widget->getOrigin() + glm::vec2(shift_amount, 0.0f));
            }
        }
    }
}

void Flexbox::render() {
    // use x and y as origin coordinates, and render everything else based off of it
    for (const auto& widget : this->widgets) {
        widget->render();
    }
}

Widget* Flexbox::borrow(Handle handle) {
    for (auto& widget : this->widgets) {
        if (widget->getHandle() == handle) {
            return widget.get();
        }
    }

    if (handle != this->handle) {
        std::cerr << "UI ERROR: Trying to borrow from Flexbox with invalid handle\n"
            << "This should never happen, and this means that we are doing something\n" 
            << "very wrong." << std::endl;
        std::exit(1);
    }

    return this;
}

bool Flexbox::hasHandle(Handle handle) const {
    for (auto& widget : this->widgets) {
        if (widget->getHandle() == handle) {
            return true;
        }
    }

    return this->handle == handle;
}

}

