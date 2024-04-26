#include "client/gui/widget/widget.hpp"

namespace gui {
namespace widget {

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
}

Widget& Widget::setAlign(HAlign halign) {
    this->setAlign(VAlign::NONE, halign);
}

Widget& Widget::setAlign(VAlign valign) {
    this->setAlign(valign, HAlign::NONE);
}

Widget& Widget::addOnClick(Callback callback, CallbackHandle& handle) {
    handle = this->next_click_handle++;
    this->on_clicks.insert({handle, callback});
}

Widget& Widget::addOnClick(Callback callback) {
    CallbackHandle handle = 0;
    this->addOnClick(callback, handle);
}

Widget& Widget::addOnHover(Callback callback, CallbackHandle& handle) {
    handle = this->next_hover_handle++;
    this->on_hovers.insert({handle, callback});
}

Widget& Widget::addOnHover(Callback callback) {
    CallbackHandle handle = 0;
    this->addOnHover(callback, handle);
}

void Widget::removeOnClick(CallbackHandle handle) {
    this->on_clicks.erase(handle);
}

void Widget::removeOnHover(CallbackHandle handle) {
    this->on_hovers.erase(handle);
}

Type Widget::getType() const {
    return this->type;
}

}
}
