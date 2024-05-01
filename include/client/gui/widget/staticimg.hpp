#pragma once

#include "client/gui/widget/widget.hpp"
#include "client/gui/img/loader.hpp"
#include "client/gui/img/img.hpp"

#include <memory>

namespace gui::widget {

class StaticImg : public Widget {
public:
    using Ptr = std::unique_ptr<StaticImg>;

    template <typename... Params>
    static Ptr make(Params&&... params) {
        return std::make_unique<StaticImg>(std::forward<Params>(params)...);
    }

    StaticImg(glm::vec2 origin, gui::img::Img img, std::shared_ptr<gui::img::Loader> loader);

    void render(GLuint shader) override;

private:
    std::shared_ptr<gui::img::Loader> loader;

};

}
