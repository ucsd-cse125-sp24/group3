#include "client/gui/widget/centertext.hpp"
#include "client/client.hpp"

namespace gui::widget {

Widget::Ptr CenterText::make(
    std::string text,
    font::Font font,
    font::Size size,
    font::Color color,
    std::shared_ptr<font::Loader> fonts,
    float y_pos
) {
    auto flex = widget::Flexbox::make(
        glm::vec2(0.0f, y_pos),
        glm::vec2(WINDOW_WIDTH, 0.0f),
        widget::Flexbox::Options(widget::Justify::VERTICAL, widget::Align::CENTER, 0.0f)
    );
    auto title = widget::DynText::make(text, fonts,
        widget::DynText::Options(font, size, color));
    flex->push(std::move(title));
    return flex;
}

}
