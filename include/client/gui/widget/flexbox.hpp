#pragma once

#include "client/gui/widget/widget.hpp"

#include <vector>
#include <memory>

namespace gui::widget {

class Flexbox : public Widget {
public:
    using Ptr = std::unique_ptr<Flexbox>;

    struct Options {
        Options(Dir direction, Align alignment, float padding):
            direction(direction), alignment(alignment), padding(padding) {}

        Dir direction;
        Align   alignment;
        float   padding;
    };

    /**
     * @brief creates a flexbox unique ptr for use in the GUI
     * 
     * NOTE: I don't think flexboxes inside of flexboxes will currently work, but I haven't tried.
     * 
     * @param origin Bottom left (x,y) coordinate of the flexbox, from which the container
     * grows upward
     * @param size (Optional) Size of the container in pixels. You should only specify a size
     * in the axis that does not grow in the direction of pushing (e.g. set a static width if you
     * have a VERTICAL flexbox).
     * @param options (Optional) Configurable options for the flexbox.
     * 
     * @returns A Flexbox widget
     */
    template <typename... Params>
    static Ptr make(Params&&... params) {
        return std::make_unique<Flexbox>(std::forward<Params>(params)...);
    }

    Flexbox(glm::vec2 origin, glm::vec2 size, Options options);
    Flexbox(glm::vec2 origin, Options options);

    void doClick(float x, float y) override;
    void doHover(float x, float y) override;

    /**
     * @brief adds a new widget to the flexbox, from the bottom up
     */
    void push(Widget::Ptr&& widget);
    /**
     * IMPORTANT: must call once you are done adding every widget to the flexbox
     * as this performs some final positioning adjustments for certain direction/alignment
     * orientations
     */
    void lock() override;

    void render() override;
    
    Widget* borrow(Handle handle) override;
    bool hasHandle(Handle handle) const override;

private:
    Options options;
    std::vector<Widget::Ptr> widgets;
};


}
