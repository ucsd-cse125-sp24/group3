#pragma once

// #include "client/core.hpp"


// Include all gui headers so everyone else just needs to include this file
#include "client/gui/widget/options.hpp"
#include "client/gui/widget/type.hpp"
#include "client/gui/widget/widget.hpp"
#include "client/gui/widget/dyntext.hpp"
#include "client/gui/widget/flexbox.hpp"
#include "client/gui/widget/staticimg.hpp"
#include "client/gui/widget/centertext.hpp"
#include "client/gui/widget/textinput.hpp"
#include "client/gui/font/font.hpp"
#include "client/gui/font/loader.hpp"
#include "client/gui/img/img.hpp"
#include "client/gui/img/loader.hpp"

#include <iostream>
#include <memory>
#include <unordered_map>

namespace gui {

enum class GUIState {
    NONE,
    TITLE_SCREEN,
    LOBBY_BROWSER,
    LOBBY,
    GAME_HUD,
    GAME_ESC_MENU
};

class GUI {
public:
    explicit GUI(Client* client);

    bool init(GLuint text_shader);

    void beginFrame();
    void layoutFrame(GUIState state);
    void handleInputs(float mouse_xpos, float mouse_ypos, bool& is_left_mouse_down);
    void renderFrame();

    widget::Handle addWidget(widget::Widget::Ptr&& widget);
    std::unique_ptr<widget::Widget> removeWidget(widget::Handle handle);

    bool shouldCaptureKeystrokes() const;
    void setCaptureKeystrokes(bool should_capture);
    void captureKeystroke(char c);
    void captureBackspace();
    std::string getCapturedKeyboardInput() const;

    template <typename W>
    W* borrowWidget(widget::Handle handle) {
        for (const auto& [_, widget] : this->widgets) {
            if (widget->hasHandle(handle)) {
                return dynamic_cast<W*>(widget->borrow(handle));
            }
        }

        std::cerr << "GUI ERROR: attempting to borrowWidget from GUI\n"
            << "with invalid handle. This should never happen\n"
            << "and means we are doing something very very bad." << std::endl;
        std::exit(1);
    }

    void handleClick(float x, float y);
    void handleHover(float x, float y);

    void clearAll();

    std::shared_ptr<font::Loader> getFonts();

private:
    widget::Handle next_handle {0};
    std::unordered_map<widget::Handle, widget::Widget::Ptr> widgets;
    GLuint text_shader;

    std::shared_ptr<font::Loader> fonts;
    img::Loader images;

    bool capture_keystrokes;
    std::string keyboard_input;

    Client* client;

    void _layoutTitleScreen();
    void _layoutLobbyBrowser();
    void _layoutLobby();
    void _layoutGameHUD();
    void _layoutGameEscMenu()
};

using namespace gui;

}