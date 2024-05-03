#pragma once

#include "client/gui/widget/widget.hpp"
#include "client/gui/img/loader.hpp"
#include "client/gui/img/img.hpp"

#include <memory>

namespace gui::widget {

class StaticImg : public Widget {
public:
    using Ptr = std::unique_ptr<StaticImg>;
    static GLuint shader;

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
