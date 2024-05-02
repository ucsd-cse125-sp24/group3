#include "client/gui/widget/textinput.hpp"
#include "client/core.hpp"
#include "client/gui/gui.hpp"
#include <string>

namespace gui::widget {

TextInput::TextInput(glm::vec2 origin,
    std::string placeholder,
    gui::GUI* gui,
    std::shared_ptr<font::Loader> fonts, 
    DynText::Options options):
    Widget(Type::TextInput, origin)
{
    std::string text_to_display;
    font::FontColor color_to_display;
    std::string captured_input = gui->getCapturedKeyboardInput();
    if (captured_input.size() == 0) {
        text_to_display = placeholder;
        options.color = font::getRGB(font::FontColor::GRAY);
    } else {
        text_to_display = captured_input;
    }

    this->dyntext = DynText::make(origin, text_to_display, fonts, options);
    this->dyntext->addOnClick([gui](widget::Handle handle) {
        gui->setCaptureKeystrokes(true);
    });
}

TextInput::TextInput(glm::vec2 origin,
    std::string placeholder,
    gui::GUI* gui,
    std::shared_ptr<font::Loader> fonts):
    TextInput(origin, placeholder, gui, fonts, DynText::Options {
        .font {font::Font::TEXT},
        .font_size {font::FontSizePx::MEDIUM},
        .color {font::getRGB(font::FontColor::BLACK)},
        .scale {1.0},
    }) {}

void TextInput::render(GLuint shader) {
    this->dyntext->render(shader);
}

bool TextInput::hasHandle(Handle handle) const {
    return handle == this->dyntext->getHandle() ||
        this->handle == handle;
}

Widget* TextInput::borrow(Handle handle) {
    if (this->handle == handle) {
        return this;
    }

    if (this->dyntext->getHandle() == handle) {
        return this->dyntext.get();
    }

    std::cerr << "UI ERROR: Trying to borrow from Text Input with invalid handle\n"
        << "This should never happen, and this means that we are doing something\n" 
        << "very wrong." << std::endl;
    std::exit(1);
}

void TextInput::doClick(float x, float y) {
    Widget::doClick(x, y);
    this->dyntext->doClick(x, y);
}

void TextInput::doHover(float x, float y) {
    Widget::doHover(x, y);
    this->dyntext->doHover(x, y);
}

}