#pragma once

#include <string>
#include <memory>

#include "client/core.hpp"
#include "client/shader.hpp"
#include "client/gui/widget/widget.hpp"
#include "client/gui/font/font.hpp"
#include "client/gui/font/loader.hpp"

namespace gui::widget {

class DynText : public Widget {
public:
    using Ptr = std::unique_ptr<DynText>;
    static std::unique_ptr<Shader> shader;

    struct Options {
        Options(font::Font font, font::Size size, font::Color color):
            font(font), size(size), color(color) {}

        font::Font font {font::Font::TEXT};
        font::Size size {font::Size::SMALL};
        font::Color color {font::Color::BLACK};
    };

    /**
     * @brief creates a DynText unique ptr widget
     * 
     * @param origin (Optional) Bottom left coordinate position of the widget in GUI coordinates
     * @param text Text to render
     * @param load Font loader
     * @param options (Optional) Options to customize how the text is rendered
     * 
     * @returns a DynText widget
     */
    template <typename... Params>
    static Ptr make(Params&&... params) {
        return std::make_unique<DynText>(std::forward<Params>(params)...);
    }

    DynText(glm::vec2 origin, std::string text, std::shared_ptr<gui::font::Loader> loader, Options options);
    DynText(std::string text, std::shared_ptr<gui::font::Loader> loader, Options options);

    void render() override;

    void changeColor(font::Color new_color);

private:
    Options options;
    std::string text;
    std::shared_ptr<gui::font::Loader> fonts;

    unsigned int VAO;
    unsigned int VBO;
};

}
