#include "client/gui/widget/flexbox.hpp"

#include <vector>
#include <initializer_list>
#include <memory>

#include "client/core.hpp"

namespace gui::widget {

void Flexbox::render(GLuint shader, float x, float y) {
    // use x and y as origin coordinates, and render everything else based off of it

    float curr_y = y;
    for (const auto& widget : this->widgets) {
        widget->render(shader, x, curr_y);
        const auto& [_, curr_height] = widget->getSize();
        curr_y += curr_height;
    }
}


}

