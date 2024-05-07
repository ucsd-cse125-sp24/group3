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

    /**
     * @brief Constructs a TextInput unique ptr widget
     * 
     * @param Origin bottom left (x,y) coordinate in GUI coordinates
     * @param placeholder Text to display if no text has been inputted yet
     * @param gui Pointer to the GUI class, so internally we can access keystroke information
     * @param fonts Font loader
     * @param options (Optional) Configurable options for the widget
     */
    template <typename... Params>
    static Ptr make(Params&&... params) {
        return std::make_unique<TextInput>(std::forward<Params>(params)...);
    }

    TextInput(glm::vec2 origin, 
        const std::string& placeholder,
        gui::GUI* gui,
        std::shared_ptr<font::Loader> fonts, 
        DynText::Options options);

    void render() override;

    bool hasHandle(Handle handle) const override;
    Widget* borrow(Handle handle) override;
    void doClick(float x, float y) override;
    void doHover(float x, float y) override;

private:
    static std::string prev_input;

    widget::DynText::Ptr dyntext;

    /// NOTE: this widget needs a pointer to the GUI so it can access the GUI's keystroke capturing functionality
    gui::GUI* gui;
};

}
