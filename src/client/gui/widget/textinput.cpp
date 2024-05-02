#include "client/gui/widget/textinput.hpp"
#include "client/core.hpp"
#include "client/gui/gui.hpp"
#include "client/client.hpp"
#include "shared/utilities/time.hpp"

#include <string>
#include <chrono>

namespace gui::widget {

std::string TextInput::prev_input = "";

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

    auto ms_since_epoch = getMsSinceEpoch();
    if (Client::getTimeOfLastKeystroke() + 1000 > ms_since_epoch || ms_since_epoch % 2000 > 1000) {
        if (gui->shouldCaptureKeystrokes()) {
            text_to_display += '|';
        }
    }

    this->dyntext = DynText::make(origin, text_to_display, fonts, options);
    this->dyntext->addOnClick([gui](widget::Handle handle) {
        gui->setCaptureKeystrokes(true);
    });

    auto [width, height] = this->dyntext->getSize();
    this->width = width;
    this->height = height;
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