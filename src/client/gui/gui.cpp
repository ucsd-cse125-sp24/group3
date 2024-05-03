#include "client/gui/gui.hpp"

#include <memory>
#include <iostream>
#include <vector>
#include "shared/utilities/rng.hpp"
#include "client/client.hpp"
#include "shared/game/sharedgamestate.hpp"


namespace gui {

GUI::GUI(Client* client) {
    this->client = client;
}

bool GUI::init(GLuint text_shader, GLuint image_shader)
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

    // Need to register all of the necessary shaders for each widget
    widget::DynText::shader = text_shader;
    widget::StaticImg::shader = image_shader;

    std::cout << "Initialized GUI\n";
    return true;
}

void GUI::beginFrame() {
    std::unordered_map<widget::Handle, widget::Widget::Ptr> empty;
    std::swap(this->widgets, empty);
}

void GUI::renderFrame() {
    // for text rendering
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);  
    glEnable(GL_CULL_FACE);
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

    for (auto& [handle, widget] : this->widgets) {
        widget->render();
    }

    glDisable(GL_CULL_FACE);
    glDisable(GL_BLEND);
}

void GUI::handleInputs(float mouse_xpos, float mouse_ypos, bool& is_left_mouse_down) {
    // convert to gui coords, where (0,0) is bottome left
    mouse_ypos = WINDOW_HEIGHT - mouse_ypos;
    if (is_left_mouse_down) {
        this->_handleClick(mouse_xpos, mouse_ypos);
        is_left_mouse_down = false;
    }
    this->_handleHover(mouse_xpos, mouse_ypos);
}

widget::Handle GUI::addWidget(widget::Widget::Ptr&& widget) {
    widget::Handle handle = this->next_handle++;
    this->widgets.insert({handle, std::move(widget)});
    return handle;
}

widget::Widget::Ptr GUI::removeWidget(widget::Handle handle) {
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

void GUI::clearCapturedKeyboardInput() {
    this->keyboard_input = "";
}



std::shared_ptr<font::Loader> GUI::getFonts() {
    return this->fonts;
}

void GUI::layoutFrame(GUIState state) {
    switch (state) {
        case GUIState::TITLE_SCREEN:
            glfwSetInputMode(client->window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
            this->_layoutTitleScreen();
            break;
        case GUIState::GAME_ESC_MENU:
            glfwSetInputMode(client->window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
            this->_layoutGameEscMenu();
            break;
        case GUIState::LOBBY_BROWSER:
            glfwSetInputMode(client->window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
            this->_layoutLobbyBrowser();
            break;
        case GUIState::GAME_HUD:
            glfwSetInputMode(client->window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
            this->_layoutGameHUD();
            break;
        case GUIState::LOBBY:
            glfwSetInputMode(client->window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
            this->_layoutLobby();
            break;
        case GUIState::NONE:
            break;
    }
}


void GUI::_layoutTitleScreen() {
    this->addWidget(widget::CenterText::make(
        "Arcana",
        font::Font::MENU,
        font::FontSizePx::HUGE,
        font::FontColor::RED,
        fonts,
        FRAC_WINDOW_HEIGHT(2, 3)
    ));

    auto start_text = widget::DynText::make(
        "Start Game",
        fonts,
        widget::DynText::Options {
            .font = font::Font::MENU,
            .font_size = font::FontSizePx::MEDIUM,
            .color = font::getRGB(font::FontColor::BLACK),
            .scale = 1.0f,
        }
    );
    start_text->addOnHover([this](widget::Handle handle) {
        auto widget = this->borrowWidget<widget::DynText>(handle);
        widget->changeColor(font::FontColor::RED);
    });
    start_text->addOnClick([this](widget::Handle handle) {
        client->gui_state = GUIState::LOBBY_BROWSER;
    });
    auto start_flex = widget::Flexbox::make(
        glm::vec2(0.0f, FRAC_WINDOW_HEIGHT(1, 3)),
        glm::vec2(WINDOW_WIDTH, 0.0f),
        widget::Flexbox::Options {
            .direction = widget::JustifyContent::VERTICAL,
            .alignment = widget::AlignItems::CENTER,
            .padding = 0.0f,
        }
    );
    start_flex->push(std::move(start_text));

    this->addWidget(std::move(start_flex));
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
            this->client->gui_state = GUIState::LOBBY;
        });
        entry->addOnHover([this](widget::Handle handle){
            auto widget = this->borrowWidget<widget::DynText>(handle);
            widget->changeColor(font::FontColor::RED);
        });
        lobbies_flex->push(std::move(entry));
    }

    if (client->lobby_finder.getFoundLobbies().empty()) {
        lobbies_flex->push(widget::DynText::make(
            "No lobbies found...",
            this->fonts,
            widget::DynText::Options {
                .font = font::Font::MENU,
                .font_size = font::FontSizePx::SMALL,
                .color = font::getRGB(font::FontColor::BLACK),
                .scale = 1.0f
            }
        ));
    }

    this->addWidget(std::move(lobbies_flex));

    this->addWidget(widget::TextInput::make(
        glm::vec2(FRAC_WINDOW_WIDTH(2, 5), FRAC_WINDOW_HEIGHT(1, 6)),
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
    auto lobby_title = widget::CenterText::make(
        this->client->gameState.lobby.name,
        font::Font::MENU,
        font::FontSizePx::LARGE,
        font::FontColor::BLACK,
        this->fonts,
        WINDOW_HEIGHT - font::FontSizePx::LARGE
    );
    this->addWidget(std::move(lobby_title));
    std::stringstream ss;
    ss << this->client->gameState.lobby.players.size() << " / " << this->client->gameState.lobby.max_players;
    auto player_count = widget::CenterText::make(
        ss.str(),
        font::Font::MENU,
        font::FontSizePx::MEDIUM,
        font::FontColor::BLACK,
        this->fonts,
        WINDOW_HEIGHT - (2 * font::FontSizePx::LARGE) - 10.0f
    );
    this->addWidget(std::move(player_count));

    auto players_flex = widget::Flexbox::make(
        glm::vec2(0.0f, FRAC_WINDOW_HEIGHT(1, 5)),
        glm::vec2(WINDOW_WIDTH, 0.0f),
        widget::Flexbox::Options {
            .direction = widget::JustifyContent::VERTICAL,
            .alignment = widget::AlignItems::CENTER,
            .padding = 10.0f
        }
    );
    for (const auto& [_eid, player_name] : this->client->gameState.lobby.players) {
        players_flex->push(widget::DynText::make(
            player_name,
            this->fonts,
            widget::DynText::Options {
                .font = font::Font::MENU,
                .font_size = font::FontSizePx::MEDIUM,
                .color = font::getRGB(font::FontColor::BLACK),
                .scale = 1.0f
            }
        ));
    }
    this->addWidget(std::move(players_flex));

    auto waiting_msg = widget::CenterText::make(
        "Waiting for players...",
        font::Font::MENU,
        font::FontSizePx::MEDIUM,
        font::FontColor::GRAY,
        this->fonts,
        30.0f
    );
    this->addWidget(std::move(waiting_msg));
}

void GUI::_layoutGameHUD() {

}

void GUI::_layoutGameEscMenu() {
    auto exit_game_txt = widget::DynText::make(
        "Exit Game",
        fonts,
        widget::DynText::Options {
            .font = font::Font::MENU,
            .font_size = font::FontSizePx::MEDIUM,
            .color = font::getRGB(font::FontColor::BLACK),
            .scale = 1.0f,
        }
    );
    exit_game_txt->addOnHover([this](widget::Handle handle) {
        auto widget = this->borrowWidget<widget::DynText>(handle);
        widget->changeColor(font::FontColor::RED);
    });
    exit_game_txt->addOnClick([this](widget::Handle handle) {
        glfwDestroyWindow(this->client->getWindow());
    });
    auto flex = widget::Flexbox::make(
        glm::vec2(0.0f, FRAC_WINDOW_HEIGHT(1, 2)),
        glm::vec2(WINDOW_WIDTH, 0.0f),
        widget::Flexbox::Options {
            .direction = widget::JustifyContent::VERTICAL,
            .alignment = widget::AlignItems::CENTER,
            .padding = 0.0f,
        }
    );
    flex->push(std::move(exit_game_txt));

    this->addWidget(std::move(flex));
}

void GUI::_handleClick(float x, float y) {
    for (const auto& [_, widget] : this->widgets) {
        widget->doClick(x, y);
    }
}

void GUI::_handleHover(float x, float y) {
    for (const auto& [_, widget] : this->widgets) {
        widget->doHover(x, y);
    }
}

}
