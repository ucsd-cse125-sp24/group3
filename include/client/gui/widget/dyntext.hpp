#pragma once

#include <string>
#include <memory>

#include "client/core.hpp"
#include "client/gui/widget/widget.hpp"
#include "client/gui/font/font.hpp"
#include "client/gui/font/loader.hpp"

namespace gui::widget {

class DynText : public Widget {
public:
    using Ptr = std::unique_ptr<DynText>;

    struct Options {
        font::Font font {font::Font::TEXT};
        font::FontSizePx font_size {font::FontSizePx::MEDIUM};
        glm::vec3 color {font::getRGB(font::FontColor::BLACK)};
        float scale {1.0};
    };

    template <typename... Params>
    static Ptr make(Params&&... params) {
        return std::make_unique<DynText>(std::forward<Params>(params)...);
    }

    DynText(glm::vec2 origin, std::string text, std::shared_ptr<gui::font::Loader> loader, Options options);
    DynText(glm::vec2 origin, std::string text, std::shared_ptr<gui::font::Loader> loader);
    DynText(std::string text, std::shared_ptr<gui::font::Loader> loader, Options options);
    DynText(std::string text, std::shared_ptr<gui::font::Loader> loader);

    void render(GLuint shader) override;


private:
    Options options;
    std::string text;
    std::shared_ptr<gui::font::Loader> fonts;

    unsigned int VAO;
    unsigned int VBO;
};

}
