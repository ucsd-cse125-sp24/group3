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
        float          padding;
    };

    template <typename... Params>
    static Ptr make(Params&&... params) {
        return std::make_unique<Flexbox>(std::forward<Params>(params)...);
    }

    Flexbox(glm::vec2 origin, glm::vec2 size, Options options);
    Flexbox(glm::vec2 origin, glm::vec2 size);
    Flexbox(glm::vec2 origin, Options options);
    explicit Flexbox(glm::vec2 origin);

    void doClick(float x, float y) override;
    void doHover(float x, float y) override;

    void push(Widget::Ptr&& widget);

    void render() override;
    
    Widget* borrow(Handle handle) override;
    bool hasHandle(Handle handle) const override;

private:
    Options options;
    std::vector<Widget::Ptr> widgets;
};


}
