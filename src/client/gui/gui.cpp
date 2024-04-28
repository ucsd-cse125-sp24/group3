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

    this->text.push_back(widget::DynText("Arcana", this->fonts));

    std::cout << "Initialized GUI\n";
    return true;
}

void GUI::render() {
    // for text rendering
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);  

    for (auto& t : this->text) {
        // originally tried 0,0 which should be bottom left corner i think
        // now trying to render it to random coordinates to see if 
        // it flickers maybe and im using the wrong coords at 0,0?
        t.render(this->text_shader,
            static_cast<float>(randomInt(-640, 640)),
            static_cast<float>(randomInt(-480, 480)));
    }

    glDisable(GL_BLEND);
}

}