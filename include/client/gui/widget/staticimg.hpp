#pragma once

#include "client/gui/widget/widget.hpp"
#include "client/gui/img/loader.hpp"
#include "client/gui/img/img.hpp"
#include "client/shader.hpp"

#include <memory>

namespace gui::widget {


/**
 * Widget to display a static image (png) to the screen.
 * 
 * BUG: This doesn't work at all currently! ASDAJSHHASHDAHHHHHHH!
 * 
 * Reference: The chapter on sprite rendering from
 * https://learnopengl.com/book/book_pdf.pdf
 */
class StaticImg : public Widget {
public:
    using Ptr = std::unique_ptr<StaticImg>;
    static std::unique_ptr<Shader> shader;

    /**
     * @brief creates a StaticImg unique ptr widget
     * 
     * @param origin (Optional) bottom left coordinate of the widget in GUI Coordinates
     * @param img Id for the image to render
     * 
     * @returns A StaticImg widget
     */
    template <typename... Params>
    static Ptr make(Params&&... params) {
        return std::make_unique<StaticImg>(std::forward<Params>(params)...);
    }

    StaticImg(glm::vec2 origin, gui::img::Img img);
    explicit StaticImg(gui::img::Img img);
    ~StaticImg();

    void render() override;

private:
    gui::img::Img img;
    GLuint quadVAO, VBO, EBO;
    GLuint texture_id;
};

}
