#pragma once

#include "client/gui/widget/widget.hpp"
#include "client/gui/widget/dyntext.hpp"
#include "client/gui/font/font.hpp"
#include "client/gui/font/loader.hpp"

#include <string>
#include <memory>

namespace gui {
    class GUI;
}

namespace gui::widget {

/**
 *  Widget to capture text input.
 *  NOTE: with the current implementation, there can only be one of these active in the GUI
 *  at one time. If there are multiple, they will both affect each other.
 */
class TextInput : public Widget {
public:
    using Ptr = std::unique_ptr<TextInput>;

    template <typename... Params>
    static Ptr make(Params&&... params) {
        return std::make_unique<TextInput>(std::forward<Params>(params)...);
    }

    TextInput(glm::vec2 origin, 
        std::string placeholder,
        gui::GUI* gui,
        std::shared_ptr<font::Loader> fonts, 
        DynText::Options options);

    TextInput(glm::vec2 origin,
        std::string placeholder,
        gui::GUI* gui,
        std::shared_ptr<font::Loader> fonts);

    void render() override;

    bool hasHandle(Handle handle) const override;
    Widget* borrow(Handle handle) override;
    void doClick(float x, float y) override;
    void doHover(float x, float y) override;

private:
    static std::string prev_input;

    widget::DynText::Ptr dyntext;
    gui::GUI* gui;
};

}
