#pragma once

#include "client/gui/widget/widget.hpp"

#include <initializer_list>
#include <vector>
#include <memory>

namespace gui::widget {

class Flexbox : public Widget {
public:
    struct Options {
        JustifyContent direction;
    };

    static std::unique_ptr<Widget> make(Options options) {
        return std::make_unique<Flexbox>(options);
    }

    Flexbox(Options options):
        Widget(Type::Flexbox), options(options)
    {
    }

    void addItem(std::unique_ptr<Widget> widget);

    void render(GLuint shader, float x, float y) override;

private:
    Options options;
    std::vector<std::unique_ptr<Widget>> widgets;
};

}
