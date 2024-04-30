#pragma once

#include "client/core.hpp"
#include "client/gui/widget/type.hpp"
#include "client/gui/widget/options.hpp"

#include <memory>
#include <string>
#include <functional>
#include <unordered_map>

namespace gui::widget {

using Callback = std::function<void()>;
using CallbackHandle = std::size_t;

class Widget {
public:
    using Ptr = std::unique_ptr<Widget>;

    explicit Widget(Type type, glm::vec2 origin);

    void setOrigin(glm::vec2 origin);
    const glm::vec2& getOrigin() const;

    CallbackHandle addOnClick(Callback callback);
    CallbackHandle addOnHover(Callback callback);

    void removeOnClick(CallbackHandle handle);
    void removeOnHover(CallbackHandle handle);

    virtual void render(GLuint shader) = 0;

    virtual void doClick(float x, float y);
    virtual void doHover(float x, float y);

    [[nodiscard]] Type getType() const;

    [[nodiscard]] std::pair<std::size_t, std::size_t> getSize() const;

protected:
    Type type;
    glm::vec2 origin;
    std::size_t width  {0};
    std::size_t height {0};

    std::unordered_map<CallbackHandle, Callback> on_clicks;
    std::unordered_map<CallbackHandle, Callback> on_hovers;

private:
    CallbackHandle next_click_handle {0};
    CallbackHandle next_hover_handle {0};

    bool _doesIntersect(float x, float y) const;
};


}
