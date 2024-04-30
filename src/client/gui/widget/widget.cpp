#include "client/gui/widget/widget.hpp"

namespace gui::widget {

Widget::Widget(Type type, glm::vec2 origin):
    type(type), origin(origin)
{

}

void Widget::setOrigin(glm::vec2 origin) {
    this->origin = origin;
}

const glm::vec2& Widget::getOrigin() const {
    return this->origin;
}

CallbackHandle Widget::addOnClick(Callback callback) {
    CallbackHandle handle = this->next_click_handle++;
    this->on_clicks.insert({handle, callback});
    return handle;
}


CallbackHandle Widget::addOnHover(Callback callback) {
    CallbackHandle handle = this->next_hover_handle++;
    this->on_hovers.insert({handle, callback});
    return handle;
}

void Widget::removeOnClick(CallbackHandle handle) {
    this->on_clicks.erase(handle);
}

void Widget::removeOnHover(CallbackHandle handle) {
    this->on_hovers.erase(handle);
}

void Widget::doClick(float x, float y) {
    if (this->_doesIntersect(x, y)) {
        for (const auto& [_handle, callback] : this->on_clicks) {
            callback();
        }
    }
}

void Widget::doHover(float x, float y) {
    if (this->_doesIntersect(x, y)) {
        for (const auto& [_handle, callback] : this->on_hovers) {
            callback();
        }
    }
}

Type Widget::getType() const {
    return this->type;
}

std::pair<std::size_t, std::size_t> Widget::getSize() const {
    return {this->width, this->height};
}

bool Widget::_doesIntersect(float x, float y) const {
    return (
        x > this->origin.x && 
        y > this->origin.y &&
        x < this->origin.x + this->width &&
        y < this->origin.y + this->height
    );
}

}
