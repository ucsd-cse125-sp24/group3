#pragma once

#include "client/gui/widget/flexbox.hpp"
#include "client/gui/widget/dyntext.hpp"

namespace gui::widget {

/**
 * This widget is a special kind of "Macro Widget" which essentially acts as a wrapper around a more
 * complex internal structure to make a piece of text that is centered horizontally across the 
 * screen.
 */
class CenterText {
public:
    /**
     * @brief Constructs a unique_ptr for a Flexbox widget that contains a DynText widget
     * 
     * @param text Text to display
     * @param font font to render the text with
     * @param size size of the font
     * @param color color of the text
     * @param fonts font loader
     * @param y_pos bottom y position of the widget in GUI coordinates
     * 
     * @returns a Flexbox widget that centers an internal DynText widget
     */
    static Widget::Ptr make(
        std::string text, 
        font::Font font, 
        font::Size size,
        font::Color color, 
        std::shared_ptr<font::Loader> fonts, 
        float y_pos
    );
};

}
