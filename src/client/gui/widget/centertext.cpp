#include "client/gui/widget/centertext.hpp"
#include "client/client.hpp"

namespace gui::widget {

Flexbox::Ptr CenterText::make(
    std::string text,
    font::Font font,
    font::FontSizePx size,
    font::FontColor color,
    std::shared_ptr<font::Loader> fonts,
    float y_pos
) {
    auto flex = widget::Flexbox::make(
        glm::vec2(0.0f, y_pos),
        glm::vec2(WINDOW_WIDTH, 0.0f),
        widget::Flexbox::Options {
            .direction = widget::JustifyContent::VERTICAL,
            .alignment = widget::AlignItems::CENTER,
            .padding   = 0.0f,
        });
    auto title = widget::DynText::make(
        text,
        fonts,
        widget::DynText::Options {
            .font  = font,
            .font_size = size,
            .color = font::getRGB(font::FontColor::BLACK),
            .scale = 1.0f
        });
    flex->push(std::move(title));
    return flex;
}

}
