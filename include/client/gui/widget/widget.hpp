#pragma once

#include "client/core.hpp"
#include "client/gui/widget/type.hpp"
#include "client/gui/widget/options.hpp"

#include <memory>
#include <unordered_set>
#include <string>
#include <functional>
#include <unordered_map>

namespace gui::widget {

class GUI;

using Handle = std::size_t;
using CallbackHandle = std::size_t;
using Callback = std::function<void(Handle)>;

class Widget {
public:
    using Ptr = std::unique_ptr<Widget>;

    explicit Widget(Type type, glm::vec2 origin);

    void setOrigin(glm::vec2 origin);
    [[nodiscard]] const glm::vec2& getOrigin() const;

    CallbackHandle addOnClick(Callback callback);
    CallbackHandle addOnHover(Callback callback);

    void removeOnClick(CallbackHandle handle);
    void removeOnHover(CallbackHandle handle);

    virtual void render(GLuint shader) = 0;

    virtual void doClick(float x, float y);
    virtual void doHover(float x, float y);

    [[nodiscard]] Type getType() const;

    [[nodiscard]] std::pair<std::size_t, std::size_t> getSize() const;

    [[nodiscard]] Handle getHandle() const;
    [[nodiscard]] virtual bool hasHandle(Handle handle) const;
    [[nodiscard]] virtual Widget* borrow(Handle handle);

protected:
    Handle handle;
    static std::size_t num_widgets;

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
