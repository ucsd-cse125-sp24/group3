#include "client/gui/widget/widget.hpp"

namespace gui::widget {

Widget::Widget(Type type):
    type(type)
{

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

void Widget::doClick() {
    for (const auto& [_handle, callback] : this->on_clicks) {
        callback();
    }
}

void Widget::doHover() {
    for (const auto& [_handle, callback] : this->on_hovers) {
        callback();
    }
}

Type Widget::getType() const {
    return this->type;
}

std::pair<std::size_t, std::size_t> Widget::getSize() const {
    return {this->width, this->height};
}

}
