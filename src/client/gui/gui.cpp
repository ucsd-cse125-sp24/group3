#include "client/gui/gui.hpp"

#include <memory>
#include <iostream>
#include <vector>
#include "shared/utilities/rng.hpp"
#include "client/client.hpp"
#include "shared/game/sharedgamestate.hpp"
#include "shared/game/sharedobject.hpp"
#include "shared/utilities/time.hpp"

namespace gui {


GUI::GUI(Client* client): capture_keystrokes(false) {
    this->client = client;
}

bool GUI::init()
{
    std::cout << "Initializing GUI...\n";

    auto shader_path = getRepoRoot() / "src" / "client" / "shaders";

    widget::DynText::shader = std::make_unique<Shader>(
        (shader_path / "text.vert").string(), (shader_path / "text.frag").string());
    if (widget::DynText::shader->getID() == 0) {
        return false;
    }

    widget::StaticImg::shader = std::make_unique<Shader>(
        (shader_path / "img.vert").string(), (shader_path / "img.frag").string());
    if (widget::StaticImg::shader->getID() == 0) {
        return false;
    }

    this->fonts = std::make_shared<font::Loader>();
    this->capture_keystrokes = false;

    if (!this->fonts->init()) {
        return false;
    }

    if (!this->images.init()) {
        return false;
    }

    std::cout << "Initialized GUI\n";
    return true;
}

void GUI::beginFrame() {
    std::unordered_map<widget::Handle, widget::Widget::Ptr> empty;
    std::swap(this->widgets, empty);
}

void GUI::renderFrame() {
    // for text rendering
    glEnable(GL_TEXTURE_2D);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);  
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

    for (auto& [handle, widget] : this->widgets) {
        widget->render();
    }

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
        case GUIState::INITIAL_LOAD:
            glfwSetInputMode(client->window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
            this->_layoutLoadingScreen();
            break;
        case GUIState::GAME_ESC_MENU:
            glfwSetInputMode(client->window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
            this->_sharedGameHUD();
            this->_layoutGameEscMenu();
            break;
        case GUIState::LOBBY_BROWSER:
            glfwSetInputMode(client->window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
            this->_layoutLobbyBrowser();
            break;
        case GUIState::GAME_HUD:
            glfwSetInputMode(client->window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

            this->_sharedGameHUD();
            this->_layoutGameHUD();
            break;
        case GUIState::LOBBY:
            glfwSetInputMode(client->window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
            this->_layoutLobby();
            break;
        case GUIState::DEAD_SCREEN:
            glfwSetInputMode(client->window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
            this->_layoutDeadScreen();
            break;
        case GUIState::RESULTS_SCREEN:
            //  TODO: fill results screen logic here
            this->_layoutResultsScreen();
            break;
        case GUIState::NONE:
            break;
    }

    for (auto& [_handle, widget] : this->widgets) {
        widget->lock();
    }
}

void GUI::_layoutLoadingScreen() {
    this->addWidget(widget::CenterText::make(
        "made by",
        font::Font::MENU,
        font::Size::MEDIUM,
        font::Color::WHITE,
        fonts,
        FRAC_WINDOW_HEIGHT(7,8)
    ));
    this->addWidget(widget::CenterText::make(
        "Torchlight Games",
        font::Font::MENU,
        font::Size::XLARGE,
        font::Color::TORCHLIGHT_GAMES,
        fonts,
        FRAC_WINDOW_HEIGHT(2,3)
    ));
    this->addWidget(widget::CenterText::make(
        "Loading...",
        font::Font::MENU,
        font::Size::MEDIUM,
        font::Color::WHITE,
        fonts,
        FRAC_WINDOW_HEIGHT(1,3)
    ));
}

void GUI::_layoutTitleScreen() {
    this->addWidget(widget::CenterText::make(
        "Arcana",
        font::Font::MENU,
        font::Size::XLARGE,
        font::Color::RED,
        fonts,
        FRAC_WINDOW_HEIGHT(2, 3)
    ));

    auto start_text = widget::DynText::make(
        "(Start Game)",
        fonts,
        widget::DynText::Options(font::Font::MENU, font::Size::MEDIUM, font::Color::BLACK)
    );
    start_text->addOnHover([this](widget::Handle handle) {
        auto widget = this->borrowWidget<widget::DynText>(handle);
        widget->changeColor(font::Color::RED);
    });
    start_text->addOnClick([this](widget::Handle handle) {
        client->gui_state = GUIState::LOBBY_BROWSER;
    });
    auto start_flex = widget::Flexbox::make(
        glm::vec2(0.0f, FRAC_WINDOW_HEIGHT(1, 3)),
        glm::vec2(WINDOW_WIDTH, 0.0f),
        widget::Flexbox::Options(widget::Dir::VERTICAL, widget::Align::CENTER, 0.0f)
    );

    start_flex->push(std::move(start_text));
    this->addWidget(std::move(start_flex));
}

void GUI::_layoutLobbyBrowser() {
    this->addWidget(widget::CenterText::make(
        "Lobbies",
        font::Font::MENU,
        font::Size::LARGE,
        font::Color::BLACK,
        this->fonts,
        WINDOW_HEIGHT - font::getFontSizePx(font::Size::LARGE)
    ));

    auto lobbies_flex = widget::Flexbox::make(
        glm::vec2(0.0f, FRAC_WINDOW_HEIGHT(1, 3)),
        glm::vec2(WINDOW_WIDTH, 0.0f),
        widget::Flexbox::Options(widget::Dir::VERTICAL, widget::Align::CENTER, 10.0f)
    );

    for (const auto& [ip, packet]: client->lobby_finder.getFoundLobbies()) {
        std::stringstream ss;
        ss << "(" << packet.lobby_name << "     " << packet.slots_taken << "/" << packet.slots_avail + packet.slots_taken << ")";

        auto entry = widget::DynText::make(ss.str(), this->fonts,
            widget::DynText::Options(font::Font::MENU, font::Size::MEDIUM, font::Color::BLACK));
        entry->addOnClick([ip, this](widget::Handle handle){
            std::cout << "Connecting to " << ip.address() << " ...\n";
            if (this->client->connectAndListen(ip.address().to_string())) {
                this->client->gui_state = GUIState::LOBBY;
                this->clearCapturedKeyboardInput();
            }
        });
        entry->addOnHover([this](widget::Handle handle){
            auto widget = this->borrowWidget<widget::DynText>(handle);
            widget->changeColor(font::Color::RED);
        });
        lobbies_flex->push(std::move(entry));
    }

    if (client->lobby_finder.getFoundLobbies().empty()) {
        lobbies_flex->push(widget::DynText::make(
            "No lobbies found...",
            this->fonts,
            widget::DynText::Options(font::Font::TEXT, font::Size::MEDIUM, font::Color::BLACK)
        ));
    }

    this->addWidget(std::move(lobbies_flex));

    auto input_flex = widget::Flexbox::make(
        glm::vec2(0.0f, font::getRelativePixels(30) + 2 * font::getFontSizePx(font::Size::MEDIUM)),
        glm::vec2(WINDOW_WIDTH, 0.0f),
        widget::Flexbox::Options(widget::Dir::HORIZONTAL, widget::Align::CENTER, font::getRelativePixels(20))
    );
    input_flex->push(widget::TextInput::make(
        glm::vec2(0.0f, 0.0f),
        "Manual IP",
        this,
        fonts,
        widget::DynText::Options(font::Font::TEXT, font::Size::MEDIUM, font::Color::BLACK)
    ));
    this->addWidget(std::move(input_flex));

    auto connect_flex = widget::Flexbox::make(
        glm::vec2(0.0f, font::getRelativePixels(30)),
        glm::vec2(WINDOW_WIDTH, 0.0f),
        widget::Flexbox::Options(widget::Dir::VERTICAL, widget::Align::CENTER, font::getRelativePixels(20))
    );

    std::stringstream ss;
    ss << "(Connect to \"" << this->getCapturedKeyboardInput() << "\")";
    auto connect_btn = widget::DynText::make(
        ss.str(),
        fonts,
        widget::DynText::Options(font::Font::MENU, font::Size::MEDIUM, font::Color::BLACK)
    );
    connect_btn->addOnHover([this](widget::Handle handle) {
        auto btn = this->borrowWidget<widget::DynText>(handle);
        btn->changeColor(font::Color::RED);
    });
    connect_btn->addOnClick([this](widget::Handle handle) {
        auto input = this->getCapturedKeyboardInput();
        if (client->connectAndListen(input)) {
            client->gui_state = GUIState::LOBBY;
            this->clearCapturedKeyboardInput();
        }
    });
    connect_flex->push(std::move(connect_btn));
    this->addWidget(std::move(connect_flex));
}

void GUI::_layoutLobby() {
    auto lobby_title = widget::CenterText::make(
        this->client->gameState.lobby.name,
        font::Font::MENU,
        font::Size::LARGE,
        font::Color::BLACK,
        this->fonts,
        WINDOW_HEIGHT - font::getFontSizePx(font::Size::LARGE)
    );
    this->addWidget(std::move(lobby_title));
    std::stringstream ss;
    ss << this->client->gameState.lobby.players.size() << " / " << this->client->gameState.lobby.max_players;
    auto player_count = widget::CenterText::make(
        ss.str(),
        font::Font::MENU,
        font::Size::MEDIUM,
        font::Color::BLACK,
        this->fonts,
        WINDOW_HEIGHT - (2 * font::getFontSizePx(font::Size::LARGE)) - 10.0f
    );
    this->addWidget(std::move(player_count));

    auto players_flex = widget::Flexbox::make(
        glm::vec2(0.0f, FRAC_WINDOW_HEIGHT(1, 5)),
        glm::vec2(WINDOW_WIDTH, 0.0f),
        widget::Flexbox::Options(widget::Dir::VERTICAL, widget::Align::CENTER, 10.0f)
    );
    for (const auto& [_eid, player_name] : this->client->gameState.lobby.players) {
        players_flex->push(widget::DynText::make(
            player_name,
            this->fonts,
            widget::DynText::Options(font::Font::MENU, font::Size::MEDIUM, font::Color::BLACK)
        ));
    }
    this->addWidget(std::move(players_flex));

    auto waiting_msg = widget::CenterText::make(
        "Waiting for players...",
        font::Font::MENU,
        font::Size::MEDIUM,
        font::Color::GRAY,
        this->fonts,
        30.0f
    );
    this->addWidget(std::move(waiting_msg));
}

void GUI::_sharedGameHUD() {
    auto self_eid = client->session->getInfo().client_eid;
    auto is_dm = client->session->getInfo().is_dungeon_master;

    // if one doesn't have, the other shouldn't
    // but just to be safe check both
    if (!self_eid.has_value() || !is_dm.has_value()) {
        return;
    }

    auto self = client->gameState.objects.at(*self_eid);

    auto inventory_size = !is_dm.value() ? self->inventoryInfo->inventory_size : self->trapInventoryInfo->inventory_size;
    auto selected = !is_dm.value() ? self->inventoryInfo->selected - 1 : self->trapInventoryInfo->selected - 1;

    std::string itemString = "";
    if (!is_dm.value()) {
        auto limit = self->inventoryInfo->usesRemaining[selected];
        std::string limittxt = "";
        if (limit != 0) {
            limittxt = " (" + std::to_string(limit) + ")";
        }

        if (self->inventoryInfo->inventory[selected] != ModelType::Frame) {
            switch (self->inventoryInfo->inventory[selected]) {
                case ModelType::HealthPotion: {
                    itemString = "Health Potion";
                    break;
                }
                case ModelType::NauseaPotion:
                case ModelType::InvincibilityPotion: {
                    itemString = "??? Potion";
                    break;
                }
                case ModelType::InvisibilityPotion: {
                    itemString = "Invisibility Potion";
                    break;
                }
                case ModelType::FireSpell: {
                    itemString = "Fireball Wand" + limittxt;
                    break;
                }
                case ModelType::HealSpell: {
                    itemString = "Healing Wand" + limittxt;
                    break;
                }
                case ModelType::TeleportSpell: {
                    itemString = "Teleport" + limittxt;
                    break;
                }
                case ModelType::Orb: {
                    itemString = "Orb";
                    break;
                }
                case ModelType::Dagger: {
                    itemString = "Dagger";
                    break;
                }
                case ModelType::Sword: {
                    itemString = "Sword";
                    break;
                }
                case ModelType::Hammer: {
                    itemString = "Hammer";
                    break;
                }
            }
        }
    } else { // DM hotbar
        if (self->trapInventoryInfo->inventory[selected] != ModelType::Frame) {
            switch (self->trapInventoryInfo->inventory[selected]) {
                case ModelType::FloorSpikeFull: {
                    itemString = "Floor Spike Full";
                    break;
                }
                case ModelType::FloorSpikeHorizontal: {
                    itemString = "Floor Spike Horizontal";
                    break;
                }
                case ModelType::FloorSpikeVertical: {
                    itemString = "Floor Spike Vertical";
                    break;
                }
                case ModelType::FireballTrap: {
                    itemString = "Fireball Trap";
                    break;
                }
                case ModelType::SpikeTrap: {
                    itemString = "Ceiling Spike Trap";
                    break;
                }
            }
        }
    }

    // Text for item description
    auto item_txt = widget::CenterText::make(
        itemString,
        font::Font::TEXT,
        font::Size::SMALL,
        font::Color::WHITE,
        fonts,
        font::getRelativePixels(70)
    );
    this->addWidget(std::move(item_txt));

    // Flexbox for the items 
    // Loading itemframe again if no item
    auto itemflex = widget::Flexbox::make(
        glm::vec2(0.0f, 0.0f),         
        glm::vec2(WINDOW_WIDTH, 0.0f),
        widget::Flexbox::Options(widget::Dir::HORIZONTAL, widget::Align::CENTER, 0.0f)
    );
    for (int i = 0; i < inventory_size; i++) {
        if (!is_dm.value()) {
            if (self->inventoryInfo->inventory[i] != ModelType::Frame) {
                switch (self->inventoryInfo->inventory[i]) {
                    case ModelType::HealthPotion: {
                        itemflex->push(widget::StaticImg::make(glm::vec2(0.0f), images.getImg(img::ImgID::HealthPotion), 2));
                        break;
                    }
                    case ModelType::NauseaPotion:
                    case ModelType::InvincibilityPotion: {
                        itemflex->push(widget::StaticImg::make(glm::vec2(0.0f), images.getImg(img::ImgID::UnknownPotion), 2));
                        break;
                    }
                    case ModelType::InvisibilityPotion: {
                        itemflex->push(widget::StaticImg::make(glm::vec2(0.0f), images.getImg(img::ImgID::InvisPotion), 2));
                        break;
                    }
                    case ModelType::FireSpell: {
                        itemflex->push(widget::StaticImg::make(glm::vec2(0.0f), images.getImg(img::ImgID::FireSpell), 2));
                        break;
                    }
                    case ModelType::HealSpell: {
                        itemflex->push(widget::StaticImg::make(glm::vec2(0.0f), images.getImg(img::ImgID::HealSpell), 2));
                        break;
                    }
                    case ModelType::TeleportSpell: {
                        itemflex->push(widget::StaticImg::make(glm::vec2(0.0f), images.getImg(img::ImgID::Scroll), 2));
                        break;
                    }
                    case ModelType::Orb: {
                        itemflex->push(widget::StaticImg::make(glm::vec2(0.0f), images.getImg(img::ImgID::Orb), 2));
                        break;
                    }
                    case ModelType::Dagger: {
                        itemflex->push(widget::StaticImg::make(glm::vec2(0.0f), images.getImg(img::ImgID::Dagger), 2));
                        break;
                    }
                    case ModelType::Sword: {
                        itemflex->push(widget::StaticImg::make(glm::vec2(0.0f), images.getImg(img::ImgID::Sword), 2));
                        break;
                    }
                    case ModelType::Hammer: {
                        itemflex->push(widget::StaticImg::make(glm::vec2(0.0f), images.getImg(img::ImgID::Hammer), 2));
                        break;
                    }
                }
            } else {
                itemflex->push(widget::StaticImg::make(glm::vec2(0.0f), images.getImg(img::ImgID::ItemFrame), 2));
            }
        } else {
            if (self->trapInventoryInfo->inventory[i] != ModelType::Frame) {
                switch (self->trapInventoryInfo->inventory[i]) {
                case ModelType::FloorSpikeFull: { // TODO: CHANGE images
                    itemflex->push(widget::StaticImg::make(glm::vec2(0.0f), images.getImg(img::ImgID::Orb), 2));
                    break;
                }
                case ModelType::FloorSpikeHorizontal: {
                    itemflex->push(widget::StaticImg::make(glm::vec2(0.0f), images.getImg(img::ImgID::Orb), 2));
                    break;
                }
                case ModelType::FloorSpikeVertical: {
                    itemflex->push(widget::StaticImg::make(glm::vec2(0.0f), images.getImg(img::ImgID::Orb), 2));
                    break;
                }
                case ModelType::FireballTrap: {
                    itemflex->push(widget::StaticImg::make(glm::vec2(0.0f), images.getImg(img::ImgID::Orb), 2));
                    break;
                }
                case ModelType::SpikeTrap: {
                    itemflex->push(widget::StaticImg::make(glm::vec2(0.0f), images.getImg(img::ImgID::Orb), 2));
                    break;
                }
                }
            }
            else {
                itemflex->push(widget::StaticImg::make(glm::vec2(0.0f), images.getImg(img::ImgID::ItemFrame), 2));
            }
        }
    }

    this->addWidget(std::move(itemflex));

    // Flexbox for the item frames
    auto frameflex = widget::Flexbox::make(
        glm::vec2(0.0f, 0.0f),          //position relative to screen
        glm::vec2(WINDOW_WIDTH, 0.0f),  //dimensions of the flexbox
        widget::Flexbox::Options(widget::Dir::HORIZONTAL, widget::Align::CENTER, 0.0f) //last one is padding
    );

    for (int i = 0; i < inventory_size; i++) {
        if (selected == i) {
            frameflex->push(widget::StaticImg::make(glm::vec2(0.0f), images.getImg(img::ImgID::SelectedFrame), 2));
        }
        else {
            frameflex->push(widget::StaticImg::make(glm::vec2(0.0f), images.getImg(img::ImgID::ItemFrame), 2));
        }
    }

    this->addWidget(std::move(frameflex));

}

void GUI::_layoutGameHUD() {
    auto self_eid = client->session->getInfo().client_eid;
    auto is_dm = client->session->getInfo().is_dungeon_master;

    // display crosshair
    auto crosshair = this->images.getImg(img::ImgID::Crosshair);

    this->addWidget(widget::StaticImg::make(
        glm::vec2((WINDOW_WIDTH / 2.0f) - (crosshair.width / 2.0f),
            (WINDOW_HEIGHT / 2.0f) - (crosshair.height / 2.0f)),
        this->images.getImg(img::ImgID::Crosshair)
    ));

    if (!self_eid.has_value()) {
        return;
    }

    if (is_dm.has_value() && is_dm.value()) {
        return;
    }

    auto self = client->gameState.objects.at(*self_eid);

    auto health_txt = widget::CenterText::make(
        std::to_string(self->stats->health.current()) + " / " + std::to_string(self->stats->health.max()),
        font::Font::MENU,
        font::Size::MEDIUM,
        font::Color::RED,
        fonts,
        font::getRelativePixels(90)
    );
    this->addWidget(std::move(health_txt));

    auto status_flex = widget::Flexbox::make(
        glm::vec2(font::getRelativePixels(20), font::getRelativePixels(20)),
        glm::vec2(0.0f, 0.0f),
        widget::Flexbox::Options(widget::Dir::VERTICAL, widget::Align::LEFT, font::getRelativePixels(5))
    );

    for (const std::string& status: self->statuses->getStatusStrings()) {
        status_flex->push(widget::DynText::make(
            status,
            fonts,
            widget::DynText::Options(font::Font::TEXT, font::Size::MEDIUM, font::Color::WHITE)
        ));
    }

    this->addWidget(std::move(status_flex));
    // Flexbox for item durations
    auto durationFlex = widget::Flexbox::make(
        glm::vec2(10.0f, FRAC_WINDOW_HEIGHT(1, 2)),
        glm::vec2(0.0f, 0.0f),
        widget::Flexbox::Options(widget::Dir::VERTICAL, widget::Align::LEFT, 0.0f)
    );

    std::unordered_map<SpecificID, std::pair<ModelType, double>>::iterator it = self->inventoryInfo->usedItems.begin();

    while (it != self->inventoryInfo->usedItems.end()) {
        auto id = it->first;
        auto type = self->inventoryInfo->usedItems[id].first;
        auto name = "";
        if (type == ModelType::InvisibilityPotion) {
            name = "Invisibility: ";
        }
        else if (type == ModelType::InvincibilityPotion) {
            name = "INVINCIBILITY: ";
        }
        else if (type == ModelType::NauseaPotion) {
            name = "Nauseous: ";
        }

        durationFlex->push(widget::DynText::make(
            name + std::to_string((int)self->inventoryInfo->usedItems[id].second),
            fonts,
            widget::DynText::Options(font::Font::MENU, font::Size::SMALL, font::Color::WHITE)));

        ++it;
    }
    this->addWidget(std::move(durationFlex));

    //  Display orb state here?

    //  Orb state breakdown:
    //  if match phase is MatchPhase::MazeExploration
    //      --> orb hasn't been found yet
    //  if match phase is MatchPhase::RelayRace
    //      --> orb has been found at least once!
    //          if the orb is currently being carried, print which player name
    //              is carrying it
    //          otherwise, "the orb is lost"
    std::string orbStateString;
    if (client->gameState.matchPhase == MatchPhase::MazeExploration) {
        orbStateString = "The Orb is hidden somewhere in the Labyrinth...";
    }
    else {
        bool orbIsCarried = false;
        for (auto [id, name] : client->gameState.lobby.players) {
            auto player = client->gameState.objects.at(id);

            if (!player.has_value())    continue;

            SharedObject playerObj = player.get();
            
            if (playerObj.inventoryInfo.get().hasOrb) {
                orbIsCarried = true;
                orbStateString = name + " has the Orb!";
                break;
            }
        }

        if (!orbIsCarried) {
            orbStateString = "The Orb has been dropped!";
        }
    }

    auto matchPhaseFlex = widget::Flexbox::make(
        glm::vec2(10, 900),
        glm::vec2(0, 0),
        widget::Flexbox::Options(widget::Dir::VERTICAL, widget::Align::LEFT, 0.0f)
    );

    matchPhaseFlex->push(widget::DynText::make(
        orbStateString,
        fonts,
        widget::DynText::Options(font::Font::TEXT, font::Size::MEDIUM, font::Color::WHITE)
    ));

    //  Add timer string
    if (client->gameState.matchPhase == MatchPhase::RelayRace) {
        std::string timerString = "Time Left: ";
        int timerSeconds = client->gameState.timesteps_left * ((float) TIMESTEP_LEN.count()) / 1000;
        timerString += std::to_string(timerSeconds);

        timerString += (timerSeconds > 1) ? " seconds" : " second";

        matchPhaseFlex->push(widget::DynText::make(
            timerString,
            fonts,
            widget::DynText::Options(font::Font::TEXT, font::Size::MEDIUM, font::Color::RED)
        ));
    }

    //  Add player deaths string
    std::string playerDeathsString = std::to_string(client->gameState.numPlayerDeaths)
        + " / " + std::to_string(PLAYER_DEATHS_TO_RELAY_RACE) 
        + " Player Deaths";

    matchPhaseFlex->push(widget::DynText::make(
        playerDeathsString,
        fonts,
        widget::DynText::Options(font::Font::TEXT, font::Size::MEDIUM, font::Color::RED)
    ));

    this->addWidget(std::move(matchPhaseFlex));

    /*auto orbStateText = widget::CenterText::make(
        orbStateString,
        font::Font::TEXT,
        font::Size::MEDIUM,
        font::Color::WHITE,
        fonts,
        font::getRelativePixels(900)
    );*/

    /*auto matchPhaseFlex = widget::Flexbox::make(
        glm::vec2(10, FRAC_WINDOW_HEIGHT(2, 3)),
        glm::vec2(0, 0),
        widget::Flexbox::Options(widget::Dir::VERTICAL, widget::Align::LEFT, 0.0f)
    );

    matchPhaseFlex->push(orbStateText);*/

    //this->addWidget(std::move(orbStateText));
}

void GUI::_layoutGameEscMenu() {
    auto exit_game_txt = widget::DynText::make(
        "(Exit Game)",
        fonts,
        widget::DynText::Options(font::Font::MENU, font::Size::MEDIUM, font::Color::BLACK)
    );
    exit_game_txt->addOnHover([this](widget::Handle handle) {
        auto widget = this->borrowWidget<widget::DynText>(handle);
        widget->changeColor(font::Color::RED);
    });
    exit_game_txt->addOnClick([this](widget::Handle handle) {
        glfwSetWindowShouldClose(this->client->getWindow(), GL_TRUE);
    });
    auto flex = widget::Flexbox::make(
        glm::vec2(0.0f, FRAC_WINDOW_HEIGHT(1, 2)),
        glm::vec2(WINDOW_WIDTH, 0.0f),
        widget::Flexbox::Options(widget::Dir::VERTICAL, widget::Align::CENTER, 0.0f)
    );
    flex->push(std::move(exit_game_txt));

    this->addWidget(std::move(flex));
}

void GUI::_layoutDeadScreen() {
    auto self_eid = client->session->getInfo().client_eid;
    if (!self_eid.has_value()) {
        return;
    }
    auto self = client->gameState.objects.at(*self_eid);

    auto time_until_respawn = (self->playerInfo->respawn_time - getMsSinceEpoch()) / 1000;

    this->addWidget(widget::CenterText::make(
        "You died...",
        font::Font::MENU,
        font::Size::LARGE,
        font::Color::RED,
        fonts,
        FRAC_WINDOW_HEIGHT(1, 2)
    ));
    this->addWidget(widget::CenterText::make(
        "Respawning in " + std::to_string(time_until_respawn),
        font::Font::TEXT,
        font::Size::MEDIUM,
        font::Color::BLACK,
        fonts,
        FRAC_WINDOW_HEIGHT(1, 3)
    ));
}

void GUI::_layoutResultsScreen() {
    auto self_eid = client->session->getInfo().client_eid;
    if (!self_eid.has_value()) {
        return;
    }

    auto self = client->gameState.objects.at(*self_eid);

    //  Add widget based on whether the player won or lost
    bool won;
    if (self->type == ObjectType::Player) {
        won = client->gameState.playerVictory;
    }
    else {
        won = !(client->gameState.playerVictory);
    }

    //std::cout << "playerVictory: " << client->gameState.playerVictory << std::endl;
    //std::cout << "won: " << won << std::endl;

    std::string result_string = won ? "Victory" : "Defeat";

    this->addWidget(widget::CenterText::make(
        result_string,
        font::Font::MENU,
        font::Size::LARGE,
        won ? font::Color::BLUE : font::Color::RED,
        fonts,
        FRAC_WINDOW_HEIGHT(1, 2)
    ));
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
