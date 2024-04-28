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
    struct Options {
        font::Font font {font::Font::TEXT};
        font::FontSizePx font_size {font::FontSizePx::MEDIUM};
        glm::vec3 color {font::getRGB(font::FontColor::BLACK)};
        float scale {1.0};
    };
    // TODO: way to make certain words within the dyntext different colors?

    DynText(std::string text, std::shared_ptr<gui::font::Loader> loader, Options options);
    DynText(std::string text, std::shared_ptr<gui::font::Loader> loader);

    void render(GLuint shader, float x, float y) override;

private:
    Options options;
    std::string text;
    std::shared_ptr<gui::font::Loader> fonts;

    unsigned int VAO;
    unsigned int VBO;
};

}
