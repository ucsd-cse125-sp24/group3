#include "client/gui/gui.hpp"

#include <memory>
#include <iostream>
#include <vector>
#include "shared/utilities/rng.hpp"
#include "client/client.hpp"


namespace gui {

GUI::GUI(Client* client) {
    this->client = client;
}

bool GUI::init(GLuint text_shader)
{
    std::cout << "Initializing GUI...\n";

    this->fonts = std::make_shared<font::Loader>();
    this->capture_keystrokes = false;

    if (!this->fonts->init()) {
        return false;
    }

    if (!this->images.init()) {
        return false;
    }

    this->text_shader = text_shader;

    std::cout << "Initialized GUI\n";
    return true;
}

void GUI::beginFrame() {
    std::unordered_map<widget::Handle, widget::Widget::Ptr> empty;
    std::swap(this->widgets, empty);
}

void GUI::layoutFrame(GUIState state) {
    switch (state) {
        case GUIState::GAME_ESC_MENU:
            break;
        case GUIState::LOBBY_BROWSER:
            break;
        case GUIState::GAME_ESC_MENU:
            break;
        case GUIState::NONE:
            break;
    }
}

void GUI::renderFrame() {
    // for text rendering
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);  
    glEnable(GL_CULL_FACE);
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

    for (auto& [handle, widget] : this->widgets) {
        widget->render(this->text_shader);
    }

    glDisable(GL_CULL_FACE);
    glDisable(GL_BLEND);
}

void GUI::handleInputs(float mouse_xpos, float mouse_ypos, bool& is_left_mouse_down) {
    if (is_left_mouse_down) {
        this->handleClick(mouse_xpos, mouse_ypos);
        is_left_mouse_down = false;
    }
    this->handleHover(mouse_xpos, mouse_ypos);
}

widget::Handle GUI::addWidget(widget::Widget::Ptr&& widget) {
    widget::Handle handle = this->next_handle++;
    this->widgets.insert({handle, std::move(widget)});
    return handle;
}

std::unique_ptr<widget::Widget> GUI::removeWidget(widget::Handle handle) {
    auto widget = std::move(this->widgets.at(handle));
    this->widgets.erase(handle);
    return widget;
}

void GUI::captureBackspace() {
    if (this->shouldCaptureKeystrokes()) {
        if (!this->keyboard_input.empty()) {
            this->keyboard_input.pop_back();
        }
    }
}

void GUI::captureKeystroke(char c) {
    if (this->shouldCaptureKeystrokes()) {
        if (c >= 32 && c <= 126) { // meaningful character
            this->keyboard_input.push_back(c);
        }
    }
}

std::string GUI::getCapturedKeyboardInput() const {
    return this->keyboard_input.c_str();
}

bool GUI::shouldCaptureKeystrokes() const {
    return this->capture_keystrokes;
}

void GUI::setCaptureKeystrokes(bool should_capture) {
    this->capture_keystrokes = should_capture;
}

// TODO: reduce copied code between these two functions

void GUI::handleClick(float x, float y) {
    // convert to gui coords, where (0,0) is bottome left
    y = WINDOW_HEIGHT - y;

    for (const auto& [_, widget] : this->widgets) {
        widget->doClick(x, y);
    }
}

void GUI::handleHover(float x, float y) {
    // convert to gui coords, where (0,0) is bottome left
    y = WINDOW_HEIGHT - y;

    for (const auto& [_, widget] : this->widgets) {
        widget->doHover(x, y);
    }
}

std::shared_ptr<font::Loader> GUI::getFonts() {
    return this->fonts;
}


void GUI::_layoutTitleScreen() {

}

void GUI::_layoutLobbyBrowser() {
    this->addWidget(widget::CenterText::make(
        "Lobbies",
        font::Font::MENU,
        font::FontSizePx::LARGE,
        font::FontColor::BLACK,
        this->fonts,
        WINDOW_HEIGHT - font::FontSizePx::LARGE
    ));

    auto lobbies_flex = widget::Flexbox::make(
        glm::vec2(0.0f, FRAC_WINDOW_HEIGHT(1, 3)),
        glm::vec2(WINDOW_WIDTH, 0.0f),
        widget::Flexbox::Options {
            .direction = widget::JustifyContent::VERTICAL,
            .alignment = widget::AlignItems::CENTER,
            .padding   = 10.0f,
        });

    for (const auto& [ip, packet]: client->lobby_finder.getFoundLobbies()) {
        std::stringstream ss;
        ss << packet.lobby_name << "     " << packet.slots_taken << "/" << packet.slots_avail + packet.slots_taken;

        auto entry = widget::DynText::make(ss.str(),
            this->fonts, widget::DynText::Options {
                .font  = font::Font::MENU,
                .font_size = font::FontSizePx::SMALL,
                .color = font::getRGB(font::FontColor::BLACK),
                .scale = 1.0f
            });
        entry->addOnClick([ip, this](widget::Handle handle){
            std::cout << "Connecting to " << ip.address() << " ...\n";
            this->client->connectAndListen(ip.address().to_string());
        });
        entry->addOnHover([this](widget::Handle handle){
            auto widget = this->borrowWidget<widget::DynText>(handle);
            widget->changeColor(font::FontColor::BLUE);
        });
        lobbies_flex->push(std::move(entry));
    }

    this->addWidget(std::move(lobbies_flex));

    this->addWidget(widget::TextInput::make(
        glm::vec2(300.0f, 300.0f),
        "Enter a name",
        this,
        fonts,
        widget::DynText::Options {
            .font = font::Font::TEXT,
            .font_size = font::FontSizePx::SMALL,
            .color = font::getRGB(font::FontColor::BLACK),
            .scale = 1.0f
        }
    ));
}

void GUI::_layoutLobby() {

}

void GUI::_layoutGameHUD() {

}

void GUI::_layoutGameEscMenu() {

}

}
