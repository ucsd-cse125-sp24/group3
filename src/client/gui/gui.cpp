#include "client/gui/gui.hpp"

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

    this->text.push_back(widget::DynText("abcdefg", this->fonts));

    std::cout << "Initialized GUI\n";
    return true;
}

void GUI::render() {
    // for text rendering
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);  
    glEnable(GL_CULL_FACE);

    for (auto& t : this->text) {
        t.render(this->text_shader, 0.0f, 0.0f);
    }

    glDisable(GL_CULL_FACE);
    glDisable(GL_BLEND);
}

}