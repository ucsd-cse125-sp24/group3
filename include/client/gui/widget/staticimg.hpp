#pragma once

#include "client/gui/widget/widget.hpp"
#include "client/gui/img/loader.hpp"
#include "client/gui/img/img.hpp"

#include <memory>

namespace gui::widget {


/**
 * Widget to display a static image (png) to the screen.
 * 
 * BUG: This doesn't work at all currently! ASDAJSHHASHDAHHHHHHH!
 */
class StaticImg : public Widget {
public:
    using Ptr = std::unique_ptr<StaticImg>;
    static GLuint shader;

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
    StaticImg(gui::img::Img img);

    void render() override;

private:
    gui::img::Img img;
    GLuint VBO, VAO, EBO;
};

}
