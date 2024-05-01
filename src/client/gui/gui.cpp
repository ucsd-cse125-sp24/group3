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

    this->text_shader = text_shader;

    auto title = widget::DynText::make("Arcana", this->fonts);
    title->addOnClick([](){std::cout << "Clickie click on title\n";});
    auto option = widget::DynText::make("Start Game", this->fonts);
    option->addOnClick([](){std::cout << "click on option\n";});

    auto flexbox = widget::Flexbox::make(
        glm::vec2(0.0f, 0.0f),
        widget::Flexbox::Options {
            .direction { widget::JustifyContent::VERTICAL },
            .alignment { widget::AlignItems::CENTER },
        });
    flexbox->push(std::move(title));
    flexbox->push(std::move(option));

    this->addWidget(std::move(flexbox), 0.0f, 0.0f);

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
        widget->render(this->text_shader);
    }

    glDisable(GL_CULL_FACE);
    glDisable(GL_BLEND);
}

WidgetHandle GUI::addWidget(widget::Widget::Ptr&& widget, float x, float y) {
    WidgetHandle handle = this->next_handle++;
    const auto& [width, height] = widget->getSize();
    this->widgets.insert({handle, std::move(widget)});
    return handle;
}

std::unique_ptr<widget::Widget> GUI::removeWidget(WidgetHandle handle) {
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

}