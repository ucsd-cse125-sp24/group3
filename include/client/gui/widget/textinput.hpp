#pragma once

#include "client/gui/widget/widget.hpp"
#include "client/gui/widget/dyntext.hpp"
#include "client/gui/font/font.hpp"
#include "client/gui/font/loader.hpp"

#include <string>
#include <memory>

namespace gui {
    class GUI;
}

namespace gui::widget {

class TextInput : public Widget {
public:
    using Ptr = std::unique_ptr<TextInput>;

    template <typename... Params>
    static Ptr make(Params&&... params) {
        return std::make_unique<TextInput>(std::forward<Params>(params)...);
    }

    TextInput(glm::vec2 origin, 
        std::string placeholder,
        gui::GUI* gui,
        std::shared_ptr<font::Loader> fonts, 
        DynText::Options options);

    TextInput(glm::vec2 origin,
        std::string placeholder,
        gui::GUI* gui,
        std::shared_ptr<font::Loader> fonts);

    void render(GLuint shader) override;

    bool hasHandle(Handle handle) const override;
    Widget* borrow(Handle handle) override;
    void doClick(float x, float y) override;
    void doHover(float x, float y) override;

private:
    widget::DynText::Ptr dyntext;
    gui::GUI* gui;
};

}
