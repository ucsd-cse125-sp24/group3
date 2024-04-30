#include "client/gui/widget/widget.hpp"

namespace gui::widget {

Widget::Widget(Type type):
    type(type)
{

}

Widget& Widget::setSize(std::size_t width, std::size_t height) {
    this->width = width;
    this->height = height;
    return *this;
}

Widget& Widget::setAlign(VAlign valign, HAlign halign) {
    this->valign = valign;
    this->halign = halign;
    return *this;
}

Widget& Widget::setAlign(HAlign halign) {
    this->setAlign(VAlign::NONE, halign);
    return *this;
}

Widget& Widget::setAlign(VAlign valign) {
    this->setAlign(valign, HAlign::NONE);
    return *this;
}

Widget& Widget::addOnClick(Callback callback, CallbackHandle& handle) {
    handle = this->next_click_handle++;
    this->on_clicks.insert({handle, callback});
    return *this;
}

Widget& Widget::addOnClick(Callback callback) {
    CallbackHandle handle = 0;
    this->addOnClick(callback, handle);
    return *this;
}

Widget& Widget::addOnHover(Callback callback, CallbackHandle& handle) {
    handle = this->next_hover_handle++;
    this->on_hovers.insert({handle, callback});
    return *this;
}

Widget& Widget::addOnHover(Callback callback) {
    CallbackHandle handle = 0;
    this->addOnHover(callback, handle);
    return *this;
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
