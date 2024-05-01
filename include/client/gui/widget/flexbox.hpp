#pragma once

#include "client/gui/widget/widget.hpp"

#include <vector>
#include <memory>

namespace gui::widget {

class Flexbox : public Widget {
public:
    using Ptr = std::unique_ptr<Flexbox>;

    struct Options {
        JustifyContent direction;
        AlignItems     alignment;
    };

    static Ptr make(glm::vec2 origin, Options options);
    static Ptr make(glm::vec2 origin);

    Flexbox(glm::vec2 origin, Options options);
    explicit Flexbox(glm::vec2 origin);

    void doClick(float x, float y) override;
    void doHover(float x, float y) override;

    void push(Widget::Ptr&& widget);

    void render(GLuint shader) override;

private:
    Options options;
    std::vector<std::unique_ptr<Widget>> widgets;
};


}
