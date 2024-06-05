#include "client/gui/gui.hpp"

#include <memory>
#include <iostream>
#include <vector>
#include "client/gui/font/font.hpp"
#include "client/gui/widget/dyntext.hpp"
#include "shared/utilities/rng.hpp"
#include "client/client.hpp"
#include "shared/game/sharedgamestate.hpp"
#include "shared/game/sharedobject.hpp"
#include "shared/utilities/time.hpp"

namespace gui {


GUI::GUI(Client* client, const GameConfig& config): capture_keystrokes(false), logo() {
    this->client = client;
    this->config = config;
    this->controlDisplayed = true; // start with help on
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

    if (!this->logo.init()) {
        return false;
    }
    
    this->recentEvents.push_back("");

    std::cout << "Initialized GUI\n";
    return true;
}

void GUI::beginFrame() {
    std::map<widget::Handle, widget::Widget::Ptr> empty;
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

void GUI::handleInputs(float mouse_xpos, float mouse_ypos, bool& is_left_mouse_down, bool& is_right_mouse_down) {
    // convert to gui coords, where (0,0) is bottome left
    mouse_ypos = WINDOW_HEIGHT - mouse_ypos;
    if (is_left_mouse_down) {
        this->_handleClick(mouse_xpos, mouse_ypos);
        is_left_mouse_down = false;
    }
    if (is_right_mouse_down) {
        is_right_mouse_down = false;
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
    if (client->config.client.fps_counter && state != GUIState::INITIAL_LOAD) {
        _layoutFPSCounter();
    }
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
        case GUIState::INTRO_CUTSCENE:
            glfwSetInputMode(client->window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
            // draw nothing
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

void GUI::_layoutFPSCounter() {
    this->addWidget(widget::DynText::make(
        glm::vec2(10, WINDOW_HEIGHT-30),
        std::to_string(client->curr_fps),
        fonts,
        widget::DynText::Options(font::Font::TEXT, font::Size::SMALL, font::Color::WHITE)
    ));
}

void GUI::displayControl() {
    this->controlDisplayed = this->controlDisplayed ? false : true;
}

void GUI::_layoutLoadingScreen() {
    static bool first = true;

    auto logo_flex = widget::Flexbox::make(glm::vec2(0,0), glm::vec2(WINDOW_WIDTH, 0),
        widget::Flexbox::Options(widget::Dir::VERTICAL, widget::Align::CENTER, 0.0f));
    logo_flex->push(widget::StaticImg::make(glm::vec2(0, 0), this->logo.getNextFrame(), 0.75f));
    this->addWidget(std::move(logo_flex));

    if (first) {
        // the first frame will be rendered before it loads everything, then everything after will
        // be once it has loaded
        this->addWidget(widget::CenterText::make(
            "Loading...",
            font::Font::MENU,
            font::Size::MEDIUM,
            font::Color::YELLOW,
            fonts,
            FRAC_WINDOW_HEIGHT(1,2)
        ));
        first = false;
    } else {
        this->addWidget(widget::CenterText::make(
            "Press any key to continue",
            font::Font::MENU,
            font::Size::MEDIUM,
            font::Color::YELLOW,
            fonts,
            FRAC_WINDOW_HEIGHT(1,2)
        ));
        first = false;
    }
}

void GUI::_layoutTitleScreen() {
    auto logo_flex = widget::Flexbox::make(glm::vec2(0, 0), glm::vec2(WINDOW_WIDTH, 0), 
        widget::Flexbox::Options(widget::Dir::VERTICAL, widget::Align::CENTER, 0.0f));
    auto logo = widget::StaticImg::make(glm::vec2(0,0), this->logo.getNextFrame(), 0.75f); 
    logo_flex->push(std::move(logo));
    this->addWidget(std::move(logo_flex));
    
    auto title_flex = widget::Flexbox::make(glm::vec2(0, FRAC_WINDOW_HEIGHT(2,3)), glm::vec2(WINDOW_WIDTH, 0), 
        widget::Flexbox::Options(widget::Dir::VERTICAL, widget::Align::CENTER, 0.0f));
    
    title_flex->push(widget::StaticImg::make(glm::vec2(0,0), images.getImg(img::ImgID::Title)));
    this->addWidget(std::move(title_flex));

    auto start_text = widget::DynText::make(
        "Play",
        fonts,
        widget::DynText::Options(font::Font::TITLE, font::Size::MEDIUM, font::Color::WHITE)
    );
    start_text->addOnHover([this](widget::Handle handle) {
        auto widget = this->borrowWidget<widget::DynText>(handle);
        widget->changeColor(font::Color::YELLOW);
    });
    start_text->addOnClick([this](widget::Handle handle) {
        client->gui_state = GUIState::LOBBY_BROWSER;
    });
    auto exit_text = widget::DynText::make(
        "Exit",
        fonts,
        widget::DynText::Options(font::Font::TITLE, font::Size::MEDIUM, font::Color::WHITE)
    );
    exit_text->addOnHover([this](widget::Handle handle) {
        auto widget = this->borrowWidget<widget::DynText>(handle);
        widget->changeColor(font::Color::RED);
    });
    exit_text->addOnClick([this](widget::Handle handle) {
        glfwSetWindowShouldClose(this->client->getWindow(), GL_TRUE);
    });
    auto menu_flex = widget::Flexbox::make(
        glm::vec2(0.0f, FRAC_WINDOW_HEIGHT(1, 2)),
        glm::vec2(WINDOW_WIDTH, 0.0f),
        widget::Flexbox::Options(widget::Dir::VERTICAL, widget::Align::CENTER, font::getRelativePixels(30))
    );

    menu_flex->push(std::move(exit_text));
    menu_flex->push(std::move(start_text));
    this->addWidget(std::move(menu_flex));
}

void GUI::_layoutLobbyBrowser() {
    this->addWidget(widget::CenterText::make(
        "Lobbies",
        font::Font::TITLE,
        font::Size::LARGE,
        font::Color::YELLOW,
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
        ss << "" << packet.lobby_name << "     (" << packet.slots_taken << "/" << packet.slots_avail + packet.slots_taken << ")";

        auto entry = widget::DynText::make(ss.str(), this->fonts,
            widget::DynText::Options(font::Font::MENU, font::Size::MEDIUM, font::Color::WHITE));
        entry->addOnClick([ip, this](widget::Handle handle){
            std::cout << "Connecting to " << ip.address() << " ...\n";
            if (this->client->connect(ip.address().to_string())) {
                this->client->gui_state = GUIState::LOBBY;
                this->clearCapturedKeyboardInput();
            }
        });
        entry->addOnHover([this](widget::Handle handle){
            auto widget = this->borrowWidget<widget::DynText>(handle);
            widget->changeColor(font::Color::YELLOW);
        });
        lobbies_flex->push(std::move(entry));
    }

    if (client->lobby_finder.getFoundLobbies().empty()) {
        lobbies_flex->push(widget::DynText::make(
            "Searching for lobbies...",
            this->fonts,
            widget::DynText::Options(font::Font::TITLE, font::Size::MEDIUM, font::Color::YELLOW)
        ));
    }

    this->addWidget(std::move(lobbies_flex));

    auto connect_flex = widget::Flexbox::make(
        glm::vec2(FRAC_WINDOW_WIDTH(1, 3), font::getRelativePixels(30)),
        glm::vec2(0.0f, 0.0f),
        widget::Flexbox::Options(widget::Dir::HORIZONTAL, widget::Align::LEFT, font::getRelativePixels(50)
    ));

    std::stringstream ss;
    auto connect_btn = widget::DynText::make(
        "Connect",
        fonts,
        widget::DynText::Options(font::Font::TITLE, font::Size::MEDIUM, font::Color::WHITE)
    );
    connect_btn->addOnHover([this](widget::Handle handle) {
        auto btn = this->borrowWidget<widget::DynText>(handle);
        btn->changeColor(font::Color::YELLOW);
    });
    connect_btn->addOnClick([this](widget::Handle handle) {
        auto input = this->getCapturedKeyboardInput();
        if (client->connect(input)) {
            client->gui_state = GUIState::LOBBY;
            this->clearCapturedKeyboardInput();
        }
    });
    connect_flex->push(std::move(connect_btn));
    connect_flex->push(widget::TextInput::make(
        glm::vec2(0.0f, 0.0f),
        "Enter IP",
        this,
        fonts,
        widget::DynText::Options(font::Font::TEXT, font::Size::MEDIUM, font::Color::WHITE)
    ));
    this->addWidget(std::move(connect_flex));
}

void GUI::_layoutLobby() {
    //  The lobby UI has 3 subsections:
    //  1.  Lobby Name
    //  2.  Player Status Table
    //  3.  Start Game Button

    /*  GUI Subsection 1:   Lobby Name  */

    //  Specify lobby title text height
    float lobby_title_height = WINDOW_HEIGHT - font::getFontSizePx(font::Size::LARGE);

    //  Create lobby title CenterText widget
    auto lobby_title = widget::CenterText::make(
        this->client->gameState.lobby.name,
        font::Font::MENU,
        font::Size::LARGE,
        font::Color::WHITE,
        this->fonts,
        lobby_title_height
    );

    //  Add lobby title CenterText widget to lobby screen
    this->addWidget(std::move(lobby_title));

    /*  GUI Subsection 2:   Player Status Table */

    //  Define table column widths
    //glm::vec3 columnWidths(400.0f, 850.0f, 400.0f);
    glm::vec3 columnWidths(font::getRelativePixelsHorizontal(400.0f), 
        font::getRelativePixelsHorizontal(900.0f),
        font::getRelativePixelsHorizontal(400.0f));

    float rowHeight = font::getFontSizePx(font::Size::LARGE);

    //  Create max_players rows in the Player Status Table
    for (int i = 0; i < this->client->gameState.lobby.max_players; i++) {
        //  Get iterating LobbyPlayer struct
        boost::optional<LobbyPlayer> lobbyPlayer =
            this->client->gameState.lobby.players[i];

        //  Create a status row for this player
        auto player_status_row = _createPlayerStatusRow(
            lobbyPlayer,
            columnWidths,
            glm::vec2(0, lobby_title_height - rowHeight * (i + 1)),
            i + 1
        );

        //  Add table row to the screen
        this->addWidget(std::move(player_status_row));
    }

    /*  GUI Subsection 3:   Start Game Button   */

    //  Create dynamic text button for the start game button
    auto start_game_button = widget::DynText::make(
        "Start Game",
        this->fonts,
        widget::DynText::Options(
            font::Font::TEXT,
            font::Size::MEDIUM,
            font::Color::WHITE
        )
    );

    //  Display differently based on whether all players in the lobby are ready
    bool allReady = true;

    for (boost::optional<LobbyPlayer> player : this->client->gameState.lobby.players) {
        if (!player.has_value() || !player.get().ready) {
            //  Either there aren't enough players in the lobby or at least
            //  one player isn't ready
            allReady = false;
            break;
        }
    }

    if (allReady) {
        start_game_button->addOnHover([this](widget::Handle handle) {
            auto widget = this->borrowWidget<widget::DynText>(handle);
            widget->changeColor(font::Color::RED);
        });

        //  Add radio button on click
        start_game_button->addOnClick([this](widget::Handle handle) {
            auto widget = this->borrowWidget<widget::DynText>(handle);

            //  Send StartGame event to the server
            this->client->session->sendEvent(Event(
                this->client->session->getInfo().client_eid.value(),
                EventType::LobbyAction,
                LobbyActionEvent(
                    LobbyActionEvent::Action::StartGame,
                    PlayerRole::Unknown
                )
            ));
        });
    }

    //  Create flexbox to contain dynamic text
    auto start_game_flex = widget::Flexbox::make(
        glm::vec2(0, 200),
        glm::vec2(WINDOW_WIDTH, 0),
        widget::Flexbox::Options(
            widget::Dir::HORIZONTAL,
            widget::Align::CENTER,
            0.0f
        )
    );

    //  Push start button text widget to flex row
    start_game_flex->push(std::move(start_game_button));

    this->addWidget(std::move(start_game_flex));
}

gui::widget::Flexbox::Ptr GUI::_createPlayerStatusRow(
    boost::optional<LobbyPlayer> lobbyPlayer, glm::vec3 columnWidths,
    glm::vec2 origin, int playerIndex) {
    //  Determine whether the player is connected to the lobby
    bool connected = lobbyPlayer.has_value();

    //  Create table row Flexbox

    //  Row Flexbox configurations
    glm::vec2 rowSize(WINDOW_WIDTH, 0);
    widget::Flexbox::Options rowFlexboxOptions(
        widget::Dir::HORIZONTAL,
        widget::Align::CENTER,
        0.0f
    );

    auto player_status_row = widget::Flexbox::make(
        origin,
        rowSize,
        rowFlexboxOptions
    );

    //  Optional: Add a left margin Empty widget here if necessary
    //  at the start of the row

    /*  Add Player Identification Column    */
    //  There are 3 possible values in the Player Identification column,
    //  all of which are strings
    //  Case 1: "Empty"
    //      This is displayed when the lobby player isn't connected
    //      (i.e., the lobbyPlayer boost::optional doesn't have a value)
    //  Case 2: "Player X"
    //      This is displayed if the given player is in the lobby and
    //      isn't this client's player
    //  Case 3: "Player X (You)"
    //      This is displayed if the given player is in the lobby and
    //      is this client's player
    std::string playerIdString;

    if (!connected) {
        //  Player in this row is not connected (Case 1)
        playerIdString = "Empty";
    }
    else {
        //  Player is connected to the lobby (Case 2 or Case 3)
        //  Note that in both of these cases, the player identification
        //  string has the same prefix, "Player X"
        playerIdString = "Player " + std::to_string(playerIndex);

        //  Check whether this player is this client's player (Case 3)
        //  This code assumes that the client's eid should always have
        //  a value if we're in GUIState::Lobby
        //  NOTE: I have gotten an exception here since that wasn't true
        //  so there might be some sort of race condition. For now, I
        //  will simply add a has_value() check ahead of this
        if (this->client->session.get()->getInfo().client_eid.has_value()
            && this->client->session.get()->getInfo().client_eid.value()
            == lobbyPlayer.get().id) {
            playerIdString += " (You)";
        }
    }

    //  Create the player identification text widget
    auto playerID = widget::DynText::make(
        playerIdString,
        this->fonts,
        widget::DynText::Options(
            font::Font::TEXT,
            font::Size::MEDIUM,
            font::Color::WHITE
        )
    );

    //  Calculate the remaining column width and create an Empty widget with this width
    float column1_empty_width = columnWidths.x - playerID->getSize().first;

    auto column1_empty = widget::Empty::make(column1_empty_width);

    //  Push both the text widget and the Empty widget to the row Flexbox
    player_status_row->push(std::move(playerID));
    player_status_row->push(std::move(column1_empty));

    //  Optional: Can add column padding here for additional padding between
    //  columns 1 and 2

    /*  Add Player Role Column  */
    //  There are two different behaviors based on whether the given lobbyPlayer
    //  is this client's player or not.

    if (!connected
        || (this->client->session.get()->getInfo().client_eid.has_value() 
            && lobbyPlayer.get().id != this->client->session.get()->getInfo().client_eid.value())) {
        //  Case 1: This player is NOT the client player
        //  In this case, there are 4 possible subcases, all of which are composed
        //  of a single text widget:
        //  Subcase 1: "..."
        //      Displays if this player isn't connected
        //  Subcase 2: "Player X isn't ready..."
        //      Displays if this player is connected but not ready
        //  Subcase 3: "Player X wants to play as the DM."
        //      Displays if this player is ready and their desired player role is
        //      PlayerRole::DungeonMaster
        //  Subcase 4: "Player X wants to play as a Player."
        //      Displays if this player is ready and their desired player role is
        //      PlayerRole::Player
        
        std::string playerRoleString;

        if (!connected) {
            //  Subcase 1
            playerRoleString = "...";
        }
        else {
            //  Subcase 2, 3, or 4
            if (!lobbyPlayer.get().ready) {
                //  Subcase 2
                playerRoleString = "Player " + std::to_string(playerIndex)
                    + " isn't ready...";
            }
            else {
                //  Subcase 3 or 4
                if (lobbyPlayer.get().desired_role == PlayerRole::DungeonMaster) {
                    //  Subcase 3
                    playerRoleString = "Player " + std::to_string(playerIndex)
                        + " wants to play as the DM.";
                }
                else if (lobbyPlayer.get().desired_role == PlayerRole::Player) {
                    //  subcase 4
                    playerRoleString = "Player " + std::to_string(playerIndex)
                        + " wants to play as a Player.";
                }
                else {
                    //  Error - player is in the ready state but their desired
                    //  role isn't the DM or a Player!
                    playerRoleString = "Error! Player " + std::to_string(playerIndex)
                        + " wants to play as ???";
                }
            }
        }

        //  Create player role text widget
        auto player_role = widget::DynText::make(
            playerRoleString,
            this->fonts,
            widget::DynText::Options(
                font::Font::TEXT,
                font::Size::MEDIUM,
                font::Color::WHITE
            )
        );

        //  Compute the remaining column width and create an Empty widget
        float column2_empty_width = columnWidths.y - player_role->getSize().first;

        auto column2_empty = widget::Empty::make(column2_empty_width);

        //  Push both the player role text widget and the Empty widget to the row
        player_status_row->push(std::move(player_role));
        player_status_row->push(std::move(column2_empty));
    }
    else {
        //  Case 2: This player IS the client player

        //  Add a text widget which says "Play as:"
        auto radio_buttons_label = widget::DynText::make(
            "Play as:",
            this->fonts,
            widget::DynText::Options(
                font::Font::TEXT,
                font::Size::MEDIUM,
                font::Color::WHITE
            )
        );

        //  Add Empty widget been radio buttons label and first radio
        //  button
        auto radio_label_first_option_empty = widget::Empty::make(
            50.0f
        );

        //  Add radio buttons "Player" and "DM"

        //  "Player" radio button
        auto player_radio_button = widget::DynText::make(
            "Player",
            fonts,
            widget::DynText::Options(
                font::Font::TEXT,
                font::Size::MEDIUM,
                font::Color::WHITE
            )
        );

        //  "DM" radio button
        auto dm_radio_button = widget::DynText::make(
            "DM",
            fonts,
            widget::DynText::Options(
                font::Font::TEXT,
                font::Size::MEDIUM,
                font::Color::WHITE
            )
        );

        //  Add radio button hover
        player_radio_button->addOnHover([this](widget::Handle handle) {
            if (client->lobbyPlayerState != Client::LobbyPlayerState::Ready) {
                auto widget = this->borrowWidget<widget::DynText>(handle);
                widget->changeColor(font::Color::RED);
            }
        });

        //  Add radio button on click
        player_radio_button->addOnClick([this](widget::Handle handle) {
            auto widget = this->borrowWidget<widget::DynText>(handle);

            //  TODO: Change button text to be surrounded by two brackets
            //  Change other radio button text to not be surrounded by brackets
            if (client->lobbyPlayerState != Client::LobbyPlayerState::Ready) {
                client->roleSelection = Client::RadioButtonState::FirstOption;
                client->lobbyPlayerState = Client::LobbyPlayerState::SelectedRole;

                //  Update this radio button text to show it's selected
                //widget->changeText("[Player]");

                //  Update dm radio button text show it's not selected
                //dm_button->changeText("DM");
            }
        });

        //  Add Empty widget between first radio button and second radio
        //  button
        auto radio_first_second_option_empty = widget::Empty::make(
            50.0f
        );

        //  Add radio button hover
        dm_radio_button->addOnHover([this](widget::Handle handle) {
            if (client->lobbyPlayerState != Client::LobbyPlayerState::Ready) {
                auto widget = this->borrowWidget<widget::DynText>(handle);
                widget->changeColor(font::Color::RED);
            }
        });

        //  Add radio button on click
        dm_radio_button->addOnClick([this](widget::Handle handle) {
            auto widget = this->borrowWidget<widget::DynText>(handle);
            
            //  TODO: Change button text to be surrounded by two brackets
            //  Change other radio button text to not be surrounded by brackets
            if (client->lobbyPlayerState != Client::LobbyPlayerState::Ready) {
                client->roleSelection = Client::RadioButtonState::SecondOption;
                client->lobbyPlayerState = Client::LobbyPlayerState::SelectedRole;

                //  Update this radio button text to show it's selected
                //widget->changeText("[DM]");

                //  Update player radio button text show it's not selected
                //player_button->changeText("Player");
            }
        });

        //  Compute the remaining column width and create an Empty widget
        float column2_empty_width = columnWidths.y
            - radio_buttons_label->getSize().first
            - radio_label_first_option_empty->getSize().first
            - player_radio_button->getSize().first
            - radio_first_second_option_empty->getSize().first
            - dm_radio_button->getSize().first;

        auto column2_empty = widget::Empty::make(column2_empty_width);
        
        //  Push all widgets to the row
        player_status_row->push(std::move(radio_buttons_label));
        player_status_row->push(std::move(radio_label_first_option_empty));
        player_status_row->push(std::move(player_radio_button));
        player_status_row->push(std::move(radio_first_second_option_empty));
        player_status_row->push(std::move(dm_radio_button));
        player_status_row->push(std::move(column2_empty));
    }

    //  Optional: Can add column padding here for additional padding between
    //  columns 2 and 3

    /*  Add Ready Status Column */
    //  There are two different behaviors based on whether the given lobbyPlayer
    //  is this client's player or not.

    if (!connected
        || (this->client->session.get()->getInfo().client_eid.has_value()
            && lobbyPlayer.get().id != this->client->session.get()->getInfo().client_eid.value())
        ) {
        //  Case 1: This player is NOT the client player
        //  In this case, there are 2 possible subcases, all of which are composed
        //  of a single text widget:
        //  Subcase 1: "..."
        //      Displays if this player isn't connected or not ready
        //  Subcase 2: "Ready!"
        //      Displays if this player is ready

        std::string readyStatusString;

        if (!connected || !lobbyPlayer.get().ready) {
            //  Subcase 1
            readyStatusString = "...";
        }
        else {
            //  Subcase 2
            readyStatusString = "Ready!";
        }

        //  Create ready status string text widget
        auto ready_status = widget::DynText::make(
            readyStatusString,
            this->fonts,
            widget::DynText::Options(
                font::Font::TEXT,
                font::Size::MEDIUM,
                font::Color::WHITE
            )
        );

        //  Compute the remaining column width and create an Empty widget
        float column3_empty_width = columnWidths.z - ready_status->getSize().first;

        auto column3_empty = widget::Empty::make(column3_empty_width);

        //  Push all widgets to the row
        player_status_row->push(std::move(ready_status));
        player_status_row->push(std::move(column3_empty));
    }
    else {
        //  Case 2: This player IS the client player

        //  In this case, there are 3 possible subcases:
        //  Subcase 1: Text widget with value "Not Ready"
        //      Displays initially (when the player's state is Connected)
        //  Subcase 2: Clickable text widget with value "Ready? (DM/Player)"
        //      Displays when the client's player is in the SelectedRole state
        //  Subcase 3: Text widget with value "Ready!"
        //      Displays when the client's player is in the Ready state.

        std::string readyStatusString;

        //  First, set string text
        switch (this->client->lobbyPlayerState) {
            case Client::LobbyPlayerState::Connected:
                //  Subcase 1
                readyStatusString = "Not Ready";
                break;
            case Client::LobbyPlayerState::SelectedRole:
                //  Subcase 2
                readyStatusString = "Ready? (";

                switch (this->client->roleSelection) {
                    case Client::RadioButtonState::FirstOption:
                        readyStatusString += "Player";
                        break;
                    case Client::RadioButtonState::SecondOption:
                        readyStatusString += "DM";
                        break;
                }

                readyStatusString += ")";

                break;
            case Client::LobbyPlayerState::Ready:
                //  Subcase 3
                readyStatusString = "Ready!";
                break;
        }

        //  Create text widget
        auto ready_status = widget::DynText::make(
            readyStatusString,
            this->fonts,
            widget::DynText::Options(
                font::Font::TEXT,
                font::Size::MEDIUM,
                font::Color::WHITE
            )
        );

        //  Add clickable property if in subcase 2
        if (this->client->lobbyPlayerState == Client::LobbyPlayerState::SelectedRole) {
            //  Add button hover
            ready_status->addOnHover([this](widget::Handle handle) {
                auto widget = this->borrowWidget<widget::DynText>(handle);
                widget->changeColor(font::Color::RED);
            });

            //  Add radio button on click
            ready_status->addOnClick([this](widget::Handle handle) {
                auto widget = this->borrowWidget<widget::DynText>(handle);

                client->lobbyPlayerState = Client::LobbyPlayerState::Ready;

                //  TODO: Send Ready Event to Server!

                //  Determine player's desired role by radio button selected
                PlayerRole desired_role;

                switch (this->client->roleSelection) {
                    case Client::RadioButtonState::FirstOption:
                        desired_role = PlayerRole::Player;
                        break;
                    case Client::RadioButtonState::SecondOption:
                        desired_role = PlayerRole::DungeonMaster;
                        break;
                }

                this->client->session->sendEvent(Event(
                    this->client->session->getInfo().client_eid.value(),
                    EventType::LobbyAction,
                    LobbyActionEvent(
                        LobbyActionEvent::Action::Ready,
                        desired_role
                    )
                ));
            });
        }

        //  Calculate remaining empty space and create Empty widget
        float column3_empty_width = columnWidths.z - ready_status->getSize().first;

        auto column3_empty = widget::Empty::make(column3_empty_width);

        //  Push both widgets to row
        player_status_row->push(std::move(ready_status));
        player_status_row->push(std::move(column3_empty));
    }

    return player_status_row;
}

void GUI::_sharedGameHUD() {
    auto self_eid = client->session->getInfo().client_eid;
    auto is_dm = client->session->getInfo().is_dungeon_master;

    // if one doesn't have, the other shouldn't
    // but just to be safe check both
    if (!self_eid.has_value() || !is_dm.has_value()) {
        return;
    }


    // Add controls Help
    if (this->controlDisplayed) {
        auto controlBG = widget::Flexbox::make(
            glm::vec2(WINDOW_WIDTH - font::getRelativePixels(360), FRAC_WINDOW_HEIGHT(1, 2) - font::getRelativePixels(10)),
            glm::vec2(0.0f, 0.0f),
            widget::Flexbox::Options(widget::Dir::VERTICAL, widget::Align::LEFT, font::getRelativePixels(10))
        );
        if (!is_dm.value()) {
            controlBG->push(widget::StaticImg::make(glm::vec2(0.0f), images.getImg(img::ImgID::HelpBG), 2));
        }
        else {
            controlBG->push(widget::StaticImg::make(glm::vec2(0.0f), images.getImg(img::ImgID::HelpDMBG), 2));
        }
        this->addWidget(std::move(controlBG));

        auto controlsFlex = widget::Flexbox::make(
            glm::vec2(WINDOW_WIDTH - font::getRelativePixels(350), FRAC_WINDOW_HEIGHT(1, 2)),
            glm::vec2(0.0f, 0.0f),
            widget::Flexbox::Options(widget::Dir::VERTICAL, widget::Align::LEFT, font::getRelativePixels(10))
        );
        auto actionsFlex = widget::Flexbox::make(
            glm::vec2(WINDOW_WIDTH - font::getRelativePixels(150), FRAC_WINDOW_HEIGHT(1, 2)),
            glm::vec2(0.0f, 0.0f),
            widget::Flexbox::Options(widget::Dir::VERTICAL, widget::Align::LEFT, font::getRelativePixels(10))
        );

        std::vector<std::pair<std::string, std::string>> controls;
        // Controls for Player
        if (!is_dm.value()) {
            controls.push_back({ "CONTROLS", " " });
            controls.push_back({ "WASD:", "Move" });
            controls.push_back({ "Left Shift:", "Sprint" });
            controls.push_back({ "Spacebar:", "Jump" });
            controls.push_back({ "Q:", "Drop Item" });
            controls.push_back({ "Left Click:", "Use Item" });
            controls.push_back({ "Mouse Wheel:", "Select Item" });
            controls.push_back({ "ESC:", "Menu" });
            controls.push_back({ "H:", "Controls" });
        }
        // Controls for DM
        else {
            controls.push_back({ "CONTROLS", " " });
            controls.push_back({ "WASD:", "Move" });
            controls.push_back({ "Left Shift:", "Zoom In" });
            controls.push_back({ "Spacebar:", "Zoom Out" });
            controls.push_back({ "Left Control:", "Boost" });
            controls.push_back({ "Left Click:", "Place Trap" });
            controls.push_back({ "Right Click:", "Rotate Trap" });
            controls.push_back({ "Mouse Wheel:", "Select Trap" });
            controls.push_back({ "ESC:", "Menu" });
            controls.push_back({ "H:", "Controls" });
        }

        for (int i = controls.size() - 1; i >= 0; i--) {
            controlsFlex->push(widget::DynText::make(controls[i].first, fonts,
                widget::DynText::Options(font::Font::TEXT, font::Size::SMALL, font::Color::WHITE))
            );
            actionsFlex->push(widget::DynText::make(controls[i].second, fonts,
                widget::DynText::Options(font::Font::TEXT, font::Size::SMALL, font::Color::WHITE))
            );
        }
        
        this->addWidget(std::move(controlsFlex));
        this->addWidget(std::move(actionsFlex));
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
                case ModelType::Mirror: {
                    itemString = "Mirror";
                    break;
                }
            }
        }
    } else { // DM hotbar
        if (self->trapInventoryInfo->inventory[selected] != ModelType::Frame) {
            switch (self->trapInventoryInfo->inventory[selected]) {
                case ModelType::FloorSpikeFull: {
                    itemString = "Floor Spike Full";

                    if (self->trapInventoryInfo->trapsInCooldown.find(CellType::FloorSpikeFull) != self->trapInventoryInfo->trapsInCooldown.end()) {
                        itemString += " (IN COOLDOWN)";
                    }
                    break;
                }
                case ModelType::FloorSpikeHorizontal: {
                    itemString = "Floor Spike Horizontal";

                    if (self->trapInventoryInfo->trapsInCooldown.find(CellType::FloorSpikeHorizontal) != self->trapInventoryInfo->trapsInCooldown.end()) {
                        itemString += " (IN COOLDOWN)";
                    }
                    break;
                }
                case ModelType::FloorSpikeVertical: {
                    itemString = "Floor Spike Vertical";

                    if (self->trapInventoryInfo->trapsInCooldown.find(CellType::FloorSpikeVertical) != self->trapInventoryInfo->trapsInCooldown.end()) {
                        itemString += " (IN COOLDOWN)";
                    }
                    
                    break;
                }
                case ModelType::SunGod: {
                    itemString = "Fireball Trap";

                    if (self->trapInventoryInfo->trapsInCooldown.find(CellType::FireballTrapUp) != self->trapInventoryInfo->trapsInCooldown.end()
                        || self->trapInventoryInfo->trapsInCooldown.find(CellType::FireballTrapLeft) != self->trapInventoryInfo->trapsInCooldown.end()
                        || self->trapInventoryInfo->trapsInCooldown.find(CellType::FireballTrapRight) != self->trapInventoryInfo->trapsInCooldown.end()
                        || self->trapInventoryInfo->trapsInCooldown.find(CellType::FireballTrapDown) != self->trapInventoryInfo->trapsInCooldown.end()) {
                        itemString += " (IN COOLDOWN)";
                    }
                    break;
                }
                case ModelType::SpikeTrap: {
                    itemString = "Ceiling Spike Trap";

                    if (self->trapInventoryInfo->trapsInCooldown.find(CellType::SpikeTrap) != self->trapInventoryInfo->trapsInCooldown.end()) {
                        itemString += " (IN COOLDOWN)";
                    }
                    break;
                }
                case ModelType::Lightning: {
                    itemString = "Lightning Bolt (10)";
                    
                    break;
                }
                case ModelType::TeleporterTrap: {
                    itemString = "Teleporter Trap";

                    if (self->trapInventoryInfo->trapsInCooldown.find(CellType::TeleporterTrap) != self->trapInventoryInfo->trapsInCooldown.end()) {
                        itemString += " (IN COOLDOWN)";
                    }


                    break;
                }
                case ModelType::ArrowTrap: {
                    itemString = "Arrow Trap";

                    if (self->trapInventoryInfo->trapsInCooldown.find(CellType::ArrowTrapUp) != self->trapInventoryInfo->trapsInCooldown.end()
                        || self->trapInventoryInfo->trapsInCooldown.find(CellType::ArrowTrapLeft) != self->trapInventoryInfo->trapsInCooldown.end()
                        || self->trapInventoryInfo->trapsInCooldown.find(CellType::ArrowTrapDown) != self->trapInventoryInfo->trapsInCooldown.end()
                        || self->trapInventoryInfo->trapsInCooldown.find(CellType::ArrowTrapRight) != self->trapInventoryInfo->trapsInCooldown.end()) {
                        itemString += " (IN COOLDOWN)";
                    }

                    break;
                }
            }
        }
    }

    auto txtHeight = font::getRelativePixels(145.0);
    auto flexHeight = font::getRelativePixels(45.0);
    if(this->config.client.presentation){
        txtHeight += font::getRelativePixels(80.0);
        flexHeight += font::getRelativePixels(85.0);
    }

    // Item Description Background
    if (itemString != "" || is_dm.value()) {
        auto itemBGFlexHeight = txtHeight - 7;
        if (!this->config.client.fullscreen) {
            itemBGFlexHeight++;
        }
        auto itemBGFlex = widget::Flexbox::make(
            glm::vec2(0.0f, itemBGFlexHeight),
            glm::vec2(WINDOW_WIDTH, 0.0f),
            widget::Flexbox::Options(widget::Dir::HORIZONTAL, widget::Align::CENTER, 0.0f)
        );
        if (!is_dm.value()) {
            itemBGFlex->push(widget::StaticImg::make(glm::vec2(0.0f), images.getImg(img::ImgID::ItemBG), 2));
        }
        else {
            itemBGFlex->push(widget::StaticImg::make(glm::vec2(0.0f), images.getImg(img::ImgID::DMTrapBG), 2));
        }
        this->addWidget(std::move(itemBGFlex));
    }

    if (!is_dm.value()) {
        txtHeight -= font::getRelativePixels(2.0);
    }

    // Text for item description
    auto item_txt = widget::CenterText::make(
        itemString,
        font::Font::TEXT,
        font::Size::SMALL,
        font::Color::WHITE,
        fonts,
        txtHeight
    );
    this->addWidget(std::move(item_txt));

    // Flexbox for the item frames
    auto frameflex = widget::Flexbox::make(
        glm::vec2(0.0f, flexHeight),          //position relative to screen
        glm::vec2(WINDOW_WIDTH, 0.0f),  //dimensions of the flexbox
        widget::Flexbox::Options(widget::Dir::HORIZONTAL, widget::Align::CENTER, 0.0f) //last one is padding
    );

    if (!is_dm.value()) {
        frameflex->push(widget::StaticImg::make(glm::vec2(0.0f), images.getImg(img::ImgID::LeftHotbar), 2));
        for (int i = 0; i < inventory_size; i++) {
            if (selected == i) {
                frameflex->push(widget::StaticImg::make(glm::vec2(0.0f), images.getImg(img::ImgID::MiddleSelected), 2));

            }
            else {
                frameflex->push(widget::StaticImg::make(glm::vec2(0.0f), images.getImg(img::ImgID::MiddleHotbar), 2));
            }
        }
        frameflex->push(widget::StaticImg::make(glm::vec2(0.0f), images.getImg(img::ImgID::RightHotbar), 2));
    } else {
        frameflex->push(widget::StaticImg::make(glm::vec2(0.0f), images.getImg(img::ImgID::DMLeftHotbar), 2));

        for (int i = 0; i < inventory_size; i++) {
            bool idxInCooldown = false;
            int cdRemaining = 0;

            if (self->trapInventoryInfo->inventory[i] != ModelType::Frame) {
                switch (self->trapInventoryInfo->inventory[i]) {
                    case ModelType::FloorSpikeFull: {
                        if (self->trapInventoryInfo->trapsInCooldown.find(CellType::FloorSpikeFull) != self->trapInventoryInfo->trapsInCooldown.end()) {
                            cdRemaining = self->trapInventoryInfo->trapsCooldown.at(CellType::FloorSpikeFull);
                            idxInCooldown = true;
                        }
                        break;
                    }
                    case ModelType::FloorSpikeVertical: {
                        if (self->trapInventoryInfo->trapsInCooldown.find(CellType::FloorSpikeVertical) != self->trapInventoryInfo->trapsInCooldown.end()) {
                            cdRemaining = self->trapInventoryInfo->trapsCooldown.at(CellType::FloorSpikeVertical);
                            idxInCooldown = true;
                        }
                        break;
                    }
                    case ModelType::FloorSpikeHorizontal: {
                        if (self->trapInventoryInfo->trapsInCooldown.find(CellType::FloorSpikeHorizontal) != self->trapInventoryInfo->trapsInCooldown.end()) {
                            cdRemaining = self->trapInventoryInfo->trapsCooldown.at(CellType::FloorSpikeHorizontal);
                            idxInCooldown = true;
                        }
                        break;
                    }
                    case ModelType::SunGod: {
                        itemString = "Fireball Trap";

                        if (self->trapInventoryInfo->trapsInCooldown.find(CellType::FireballTrapUp) != self->trapInventoryInfo->trapsInCooldown.end()
                            || self->trapInventoryInfo->trapsInCooldown.find(CellType::FireballTrapLeft) != self->trapInventoryInfo->trapsInCooldown.end()
                            || self->trapInventoryInfo->trapsInCooldown.find(CellType::FireballTrapRight) != self->trapInventoryInfo->trapsInCooldown.end()
                            || self->trapInventoryInfo->trapsInCooldown.find(CellType::FireballTrapDown) != self->trapInventoryInfo->trapsInCooldown.end()) 
                        {
                            if (self->trapInventoryInfo->trapsInCooldown.find(CellType::FireballTrapUp) != self->trapInventoryInfo->trapsInCooldown.end()) {
                                cdRemaining = self->trapInventoryInfo->trapsCooldown.at(CellType::FireballTrapUp);
                            }
                            else if (self->trapInventoryInfo->trapsInCooldown.find(CellType::FireballTrapLeft) != self->trapInventoryInfo->trapsInCooldown.end()) {
                                cdRemaining = self->trapInventoryInfo->trapsCooldown.at(CellType::FireballTrapLeft);
                            }
                            else if (self->trapInventoryInfo->trapsInCooldown.find(CellType::FireballTrapRight) != self->trapInventoryInfo->trapsInCooldown.end()) {
                                cdRemaining = self->trapInventoryInfo->trapsCooldown.at(CellType::FireballTrapRight);
                            }
                            else {
                                cdRemaining = self->trapInventoryInfo->trapsCooldown.at(CellType::FireballTrapDown);
                            }
                            
                            idxInCooldown = true;
                        }
                        break;
                    }
                    case ModelType::SpikeTrap: {
                        itemString = "Ceiling Spike Trap";

                        if (self->trapInventoryInfo->trapsInCooldown.find(CellType::SpikeTrap) != self->trapInventoryInfo->trapsInCooldown.end()) {
                            cdRemaining = self->trapInventoryInfo->trapsCooldown.at(CellType::SpikeTrap);
                            idxInCooldown = true;
                        }
                        break;
                    }
                    case ModelType::Lightning: {
                        itemString = "Lightning Bolt (10)";

                        break;
                    }
                    case ModelType::TeleporterTrap: {
                        itemString = "Teleporter Trap";

                        if (self->trapInventoryInfo->trapsInCooldown.find(CellType::TeleporterTrap) != self->trapInventoryInfo->trapsInCooldown.end()) {
                            cdRemaining = self->trapInventoryInfo->trapsCooldown.at(CellType::TeleporterTrap);
                            idxInCooldown = true;
                        }

                        break;
                    }
                    case ModelType::ArrowTrap: {
                        itemString = "Arrow Trap";

                        if (self->trapInventoryInfo->trapsInCooldown.find(CellType::ArrowTrapUp) != self->trapInventoryInfo->trapsInCooldown.end()
                            || self->trapInventoryInfo->trapsInCooldown.find(CellType::ArrowTrapLeft) != self->trapInventoryInfo->trapsInCooldown.end()
                            || self->trapInventoryInfo->trapsInCooldown.find(CellType::ArrowTrapDown) != self->trapInventoryInfo->trapsInCooldown.end()
                            || self->trapInventoryInfo->trapsInCooldown.find(CellType::ArrowTrapRight) != self->trapInventoryInfo->trapsInCooldown.end()) 
                        {
                            if (self->trapInventoryInfo->trapsInCooldown.find(CellType::ArrowTrapUp) != self->trapInventoryInfo->trapsInCooldown.end()) {
                                cdRemaining = self->trapInventoryInfo->trapsCooldown.at(CellType::ArrowTrapUp);
                            }
                            else if (self->trapInventoryInfo->trapsInCooldown.find(CellType::ArrowTrapLeft) != self->trapInventoryInfo->trapsInCooldown.end()) {
                                cdRemaining = self->trapInventoryInfo->trapsCooldown.at(CellType::ArrowTrapLeft);
                            }
                            else if (self->trapInventoryInfo->trapsInCooldown.find(CellType::ArrowTrapDown) != self->trapInventoryInfo->trapsInCooldown.end()) {
                                cdRemaining = self->trapInventoryInfo->trapsCooldown.at(CellType::ArrowTrapDown);
                            }
                            else {
                                cdRemaining = self->trapInventoryInfo->trapsCooldown.at(CellType::ArrowTrapRight);
                            }
                            idxInCooldown = true;
                        }

                        break;
                    }
                }
            }
            
            if (!idxInCooldown) {
                if (selected == i) {
                    frameflex->push(widget::StaticImg::make(glm::vec2(0.0f), images.getImg(img::ImgID::DMMiddleSelected), 2));

                }
                else {
                    frameflex->push(widget::StaticImg::make(glm::vec2(0.0f), images.getImg(img::ImgID::DMMiddleHotbar), 2));
                }
            }
            else {
                auto imgSelect = img::ImgID::DMCD_10;
                if (cdRemaining > 4500) {
                    imgSelect = (selected != i) ? img::ImgID::DMCD_10 : img::ImgID::DMCD_Selected_10;
                } 
                else if (cdRemaining > 4000) {
                    imgSelect = (selected != i) ? img::ImgID::DMCD_9 : img::ImgID::DMCD_Selected_9;
                }
                else if (cdRemaining > 3500) {
                    imgSelect = (selected != i) ? img::ImgID::DMCD_8 : img::ImgID::DMCD_Selected_8;
                }
                else if (cdRemaining > 3000) {
                    imgSelect = (selected != i) ? img::ImgID::DMCD_7 : img::ImgID::DMCD_Selected_7;
                }
                else if (cdRemaining > 2500) {
                    imgSelect = (selected != i) ? img::ImgID::DMCD_6 : img::ImgID::DMCD_Selected_6;
                }
                else if (cdRemaining > 2000) {
                    imgSelect = (selected != i) ? img::ImgID::DMCD_5 : img::ImgID::DMCD_Selected_5;
                }
                else if (cdRemaining > 1500) {
                    imgSelect = (selected != i) ? img::ImgID::DMCD_4 : img::ImgID::DMCD_Selected_4;
                }
                else if (cdRemaining > 1000) {
                    imgSelect = (selected != i) ? img::ImgID::DMCD_3 : img::ImgID::DMCD_Selected_3;
                }
                else {
                    // Doesn't use DMCD_1 b/c it looks better without it
                    imgSelect = (selected != i) ? img::ImgID::DMCD_2 : img::ImgID::DMCD_Selected_2;
                }
                frameflex->push(widget::StaticImg::make(glm::vec2(0.0f), images.getImg(imgSelect), 2));
            }
        }
        frameflex->push(widget::StaticImg::make(glm::vec2(0.0f), images.getImg(img::ImgID::DMRightHotbar), 2));
    }
    this->addWidget(std::move(frameflex));

    // Flexbox for the items 
    // Loading itemframe again if no item
    auto itemflex = widget::Flexbox::make(
        glm::vec2(0.0f, flexHeight),
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
                    case ModelType::Mirror: {
                        //  TODO: Replace with an img of a mirror
                        itemflex->push(widget::StaticImg::make(glm::vec2(0.0f), images.getImg(img::ImgID::Mirror), 2));
                    }
                }
            } else {
                itemflex->push(widget::StaticImg::make(glm::vec2(0.0f), images.getImg(img::ImgID::Blank), 2));
            }
        } else {
            if (self->trapInventoryInfo->inventory[i] != ModelType::Frame) {
                switch (self->trapInventoryInfo->inventory[i]) {
                case ModelType::FloorSpikeFull: { // TODO: CHANGE images
                    itemflex->push(widget::StaticImg::make(glm::vec2(0.0f), images.getImg(img::ImgID::FloorSpikeTrap), 2));
                    break;
                }
                case ModelType::FloorSpikeHorizontal: {
                    itemflex->push(widget::StaticImg::make(glm::vec2(0.0f), images.getImg(img::ImgID::Teleporter), 2));
                    break;
                }
                case ModelType::FloorSpikeVertical: {
                    itemflex->push(widget::StaticImg::make(glm::vec2(0.0f), images.getImg(img::ImgID::ArrowTrap), 2));
                    break;
                }
                case ModelType::SunGod: {
                    itemflex->push(widget::StaticImg::make(glm::vec2(0.0f), images.getImg(img::ImgID::Sungod), 2));
                    break;
                }
                case ModelType::SpikeTrap: {
                    itemflex->push(widget::StaticImg::make(glm::vec2(0.0f), images.getImg(img::ImgID::SpikeTrap), 2));
                    break;
                }
                case ModelType::Lightning: {
                    itemflex->push(widget::StaticImg::make(glm::vec2(0.0f), images.getImg(img::ImgID::Lightning), 2));
                    break;
                }
                case ModelType::ArrowTrap: {
                    itemflex->push(widget::StaticImg::make(glm::vec2(0.0f), images.getImg(img::ImgID::ArrowTrap), 2));
                    break;
                }
                case ModelType::TeleporterTrap: {
                    itemflex->push(widget::StaticImg::make(glm::vec2(0.0f), images.getImg(img::ImgID::Teleporter), 2));
                    break;
                }
                }
            }
            else {
                itemflex->push(widget::StaticImg::make(glm::vec2(0.0f), images.getImg(img::ImgID::Blank), 2));
            }
        }
    }

    this->addWidget(std::move(itemflex));

    auto barHeight = font::getRelativePixels(5.0);
    if (this->config.client.presentation) {
        barHeight += font::getRelativePixels(90.0);
    }
    if (!is_dm.value()) {
        // Flexbox for the health bar
        auto healthflex = widget::Flexbox::make(
            glm::vec2(0.0f, barHeight),
            glm::vec2(WINDOW_WIDTH, 0.0f),
            widget::Flexbox::Options(widget::Dir::HORIZONTAL, widget::Align::CENTER, 0.0f)
        );
        healthflex->push(widget::StaticImg::make(glm::vec2(0.0f), images.getImg(img::ImgID::HealthBar), 2));
        this->addWidget(std::move(healthflex));

        auto healthtickflex = widget::Flexbox::make(
            glm::vec2(0.0f, barHeight),
            glm::vec2(WINDOW_WIDTH, 0.0f),
            widget::Flexbox::Options(widget::Dir::HORIZONTAL, widget::Align::CENTER, 0.0f)
        );
        for (int i = 1; i <= self->stats->health.max(); i++) {
            if (i <= self->stats->health.current()) {
                healthtickflex->push(widget::StaticImg::make(glm::vec2(0.0f), images.getImg(img::ImgID::HealthTickFull), 2));
            }
            else {
                healthtickflex->push(widget::StaticImg::make(glm::vec2(0.0f), images.getImg(img::ImgID::HealthTickEmpty), 2));
            }
        }

        this->addWidget(std::move(healthtickflex));
    }
    else {
        // Flexbox for the mana bar for DM
        auto manaflex = widget::Flexbox::make(
            glm::vec2(0.0f, barHeight),
            glm::vec2(WINDOW_WIDTH, 0.0f),
            widget::Flexbox::Options(widget::Dir::HORIZONTAL, widget::Align::CENTER, 0.0f)
        );
        manaflex->push(widget::StaticImg::make(glm::vec2(0.0f), images.getImg(img::ImgID::ManaBar), 2));
        this->addWidget(std::move(manaflex));

        auto manatickflex = widget::Flexbox::make(
            glm::vec2(0.0f, barHeight),
            glm::vec2(WINDOW_WIDTH, 0.0f),
            widget::Flexbox::Options(widget::Dir::HORIZONTAL, widget::Align::CENTER, 0.0f)
        );
        for (int i = 1; i <= 30; i++) {
            if (i <= self->DMInfo->mana_remaining) {
                manatickflex->push(widget::StaticImg::make(glm::vec2(0.0f), images.getImg(img::ImgID::ManaTickFull), 2));
            }
            else {
                manatickflex->push(widget::StaticImg::make(glm::vec2(0.0f), images.getImg(img::ImgID::ManaTickEmpty), 2));
            }
        }

        this->addWidget(std::move(manatickflex));
    }
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

    auto self = client->gameState.objects.at(*self_eid);

    auto eventBGHeight = 0;
    auto eventTxtHeight = font::getRelativePixels(25);
    if (this->config.client.presentation) {
        eventBGHeight += font::getRelativePixels(85.0);
        eventTxtHeight += font::getRelativePixels(85.0);
    }

    auto matchPhaseBGFlex = widget::Flexbox::make(
        glm::vec2(font::getRelativePixels(5), eventBGHeight),
        glm::vec2(0, 0),
        widget::Flexbox::Options(widget::Dir::VERTICAL, widget::Align::LEFT, font::getRelativePixels(10))
    );
    auto bgSize = 2.5;
    if (!is_dm.value()) {
        matchPhaseBGFlex->push(widget::StaticImg::make(glm::vec2(0.0f), images.getImg(img::ImgID::EventBG), bgSize));
    }
    else {
        matchPhaseBGFlex->push(widget::StaticImg::make(glm::vec2(0.0f), images.getImg(img::ImgID::DMEventBG), bgSize));
    }
    this->addWidget(std::move(matchPhaseBGFlex));

    auto matchPhaseFlex = widget::Flexbox::make(
        glm::vec2(font::getRelativePixels(25), eventTxtHeight),
        glm::vec2(0, 0),
        widget::Flexbox::Options(widget::Dir::VERTICAL, widget::Align::LEFT, font::getRelativePixels(7))
    );

    std::optional<glm::vec3> orb_pos;
    std::string orbStateString;
    if (client->gameState.matchPhase == MatchPhase::MazeExploration) {
        orbStateString = "The Orb is hidden...";
    }
    else {
        bool orbIsCarried = false;

        for (int i = 0; i < client->gameState.lobby.max_players; i++) {
            auto lobbyPlayer = client->gameState.lobby.players[i];

            if (!lobbyPlayer.has_value()) continue;

            auto player = client->gameState.objects.at(lobbyPlayer.get().id);

            if (!player.has_value()) continue;

            SharedObject playerObj = player.get();

            if (playerObj.type == ObjectType::DungeonMaster) continue;

            if (playerObj.inventoryInfo.get().hasOrb) {
                orbIsCarried = true;
                orbStateString = "Player " + std::to_string(i + 1) + " has the Orb!";
                break;
            }
        }

        if (!orbIsCarried) {
            orbStateString = "The Orb has been dropped!";
        }

        for (const auto& [eid, obj] : client->gameState.objects) {
            if (!obj.has_value()) {
                continue;
            }
            if (obj->type == ObjectType::Player && obj->inventoryInfo->hasOrb) {
                orb_pos = obj->physics.corner;
                orb_pos->y = 0;
                break;
            }
        }

        if (!orb_pos.has_value()) {
            for (const auto& [eid, obj] : client->gameState.objects) {
                if (!obj.has_value()) {
                    continue;
                }
                if (obj->type == ObjectType::Orb) {
                    orb_pos = obj->physics.corner;
                    orb_pos->y = 0;
                    break;
                }
            }
        }

        if (!orb_pos.has_value()) {
            std::cerr << "WARNING: orb pos does not have value... Bruh.\n";
        }
        else {
            auto player_pos_ground = self->physics.corner;
            player_pos_ground.y = 0;

            auto distance = glm::distance(orb_pos.value(), player_pos_ground);

            std::stringstream ss;

            // bruh
            ss << std::fixed << std::setprecision(1) << distance << "m to Orb.";

            const float MAX_DIST = 150.0f;
            float dist_frac = std::max(std::min(distance / MAX_DIST, 1.0f), 0.0f);

            glm::vec3 close_color = font::getRGB(font::Color::GREEN);
            glm::vec3 far_color = font::getRGB(font::Color::RED);

            glm::vec3 color = (dist_frac * far_color) + ((1 - dist_frac) * close_color);

            
            matchPhaseFlex->push(widget::DynText::make(ss.str(), fonts,
                widget::DynText::Options(font::Font::TEXT, font::Size::SMALLMEDIUM, color))
            );
        }
    }

    auto eventSize = this->recentEvents.size();
    if (this->recentEvents[eventSize - 1] != orbStateString) {
        if (eventSize >= 5) {
            this->recentEvents.erase(this->recentEvents.begin());
        }
        this->recentEvents.push_back(orbStateString);
    }
    
    for (int i = this->recentEvents.size() - 1; i >= 0; i--) {
        if (i == this->recentEvents.size() - 1) {
            matchPhaseFlex->push(widget::DynText::make(
                this->recentEvents[i],
                fonts,
                widget::DynText::Options(font::Font::TEXT, font::Size::SMALLMEDIUM, font::Color::WHITE)
            ));
        }
        else {
            matchPhaseFlex->push(widget::DynText::make(
                this->recentEvents[i],
                fonts,
                widget::DynText::Options(font::Font::TEXT, font::Size::SMALL, font::Color::GRAY)
            ));
        }
    }
    this->addWidget(std::move(matchPhaseFlex));

    // Show death or timer on the top
    auto death_flexBG = widget::Flexbox::make(
        glm::vec2(0, WINDOW_HEIGHT - (font::getRelativePixels(100))),
        glm::vec2(WINDOW_WIDTH, 0),
        widget::Flexbox::Options(widget::Dir::HORIZONTAL, widget::Align::CENTER, 0.0f)
    );
    death_flexBG->push(widget::StaticImg::make(glm::vec2(0.0f), images.getImg(img::ImgID::SkullBG), 2));
    this->addWidget(std::move(death_flexBG));

    //  Add timer string
    if (client->gameState.matchPhase == MatchPhase::RelayRace) {
        auto remainingTime = client->gameState.relay_finish_time - std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
        int min = (int) remainingTime / 60;
        int sec = remainingTime % 60;

        auto timeFlex = widget::Flexbox::make(
            glm::vec2(0, WINDOW_HEIGHT - (font::getRelativePixels(70))),
            glm::vec2(WINDOW_WIDTH, 0),
            widget::Flexbox::Options(widget::Dir::HORIZONTAL, widget::Align::CENTER, 0.0f)
        );

        std::string timerString = std::to_string(min) + "min " + std::to_string(sec) + "sec ";

        float time_frac = remainingTime / 300.0f;
        glm::vec3 close_color = font::getRGB(font::Color::GREEN);
        glm::vec3 far_color = font::getRGB(font::Color::RED);
        glm::vec3 color = ((1- time_frac) * far_color) + (time_frac * close_color);

        timeFlex->push(widget::DynText::make(
            timerString,
            fonts,
            widget::DynText::Options(font::Font::TEXT, font::Size::MEDIUM, color)
        ));
        this->addWidget(std::move(timeFlex));
    }
    else {
        auto death_flex = widget::Flexbox::make(
            glm::vec2(0, WINDOW_HEIGHT - (font::getRelativePixels(95))),
            glm::vec2(WINDOW_WIDTH, 0),
            widget::Flexbox::Options(widget::Dir::HORIZONTAL, widget::Align::CENTER, 0.0f)
        );
        for(int i = 0; i < PLAYER_DEATHS_TO_RELAY_RACE; i++){
            if(PLAYER_DEATHS_TO_RELAY_RACE - client->gameState.numPlayerDeaths > i){
                death_flex->push(widget::StaticImg::make(glm::vec2(0.0f), images.getImg(img::ImgID::Skull), 2));
            } else {
                death_flex->push(widget::StaticImg::make(glm::vec2(0.0f), images.getImg(img::ImgID::DestroyedSkull), 2));
            }  
        }
        this->addWidget(std::move(death_flex));
    }

    auto txtHeight = font::getRelativePixels(163);
    auto bartxtHeight = font::getRelativePixels(13);
    if (this->config.client.presentation) {
        txtHeight += font::getRelativePixels(80.0);
        bartxtHeight += font::getRelativePixels(90.0);
    }

    if (is_dm.has_value() && is_dm.value()) {
        // add some DM specific stuff in here
        auto traps_placed_txt = widget::CenterText::make(
            "Traps Placed: " + std::to_string(self->trapInventoryInfo->trapsPlaced) + " / " + std::to_string(MAX_TRAPS),
            font::Font::TEXT,
            font::Size::SMALL,
            font::Color::YELLOW,
            fonts,
            txtHeight
        );
        this->addWidget(std::move(traps_placed_txt));

        auto mana_txt = widget::CenterText::make(
            std::to_string(self->DMInfo->mana_remaining) + " / " + std::to_string(30),
            font::Font::TEXT,
            font::Size::SMALL,
            font::Color::WHITE,
            fonts,
            bartxtHeight
        );
        this->addWidget(std::move(mana_txt));

        //  Show a large splash text at the center of the screen for the DM if the DM is paralyzed
        if (self.get().DMInfo.get().paralyzed) {
            std::cout << "DM is paralyzed!" << std::endl;
            auto paralyzedSplashText = widget::CenterText::make(
                "PARALYZED!",
                gui::font::Font::TITLE,
                gui::font::Size::LARGE,
                gui::font::Color::RED,
                this->fonts,
                WINDOW_HEIGHT / 2
            );

            this->addWidget(std::move(paralyzedSplashText));
        }

        return;
    }

    auto health_txt = widget::CenterText::make(
        std::to_string(self->stats->health.current()) + " / " + std::to_string(self->stats->health.max()),
        font::Font::TEXT,
        font::Size::SMALL,
        font::Color::WHITE,
        fonts,
        bartxtHeight
    );
    this->addWidget(std::move(health_txt));

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
        else if (type == ModelType::Mirror) {
            name = "Holding Mirror: ";
        }

        durationFlex->push(widget::DynText::make(
            name + std::to_string((int)self->inventoryInfo->usedItems[id].second),
            fonts,
            widget::DynText::Options(font::Font::MENU, font::Size::SMALL, font::Color::WHITE)));

        ++it;
    }
    this->addWidget(std::move(durationFlex));

    auto compassHeight = font::getRelativePixels(10);
    if (this->config.client.presentation) {
        compassHeight += font::getRelativePixels(85.0);
    }

    auto compassFlex = widget::Flexbox::make(
        glm::vec2(WINDOW_WIDTH - font::getRelativePixelsHorizontal(700), compassHeight),
        glm::vec2(0.0f, 0.0f),
        widget::Flexbox::Options(widget::Dir::VERTICAL, widget::Align::LEFT, font::getRelativePixels(5))
    );

    if (self->compass->angle > 345 || self->compass->angle <= 15) {
        compassFlex->push(widget::StaticImg::make(glm::vec2(0.0f), images.getImg(img::ImgID::Compass0), 2));
    }
    else if (self->compass->angle > 15 && self->compass->angle <= 45) {
        compassFlex->push(widget::StaticImg::make(glm::vec2(0.0f), images.getImg(img::ImgID::Compass330), 2));
    }
    else if (self->compass->angle > 45 && self->compass->angle <= 75) {
        compassFlex->push(widget::StaticImg::make(glm::vec2(0.0f), images.getImg(img::ImgID::Compass300), 2));
    }
    else if (self->compass->angle > 75 && self->compass->angle <= 105) {
        compassFlex->push(widget::StaticImg::make(glm::vec2(0.0f), images.getImg(img::ImgID::Compass270), 2));
    }
    else if (self->compass->angle > 105 && self->compass->angle <= 135) {
        compassFlex->push(widget::StaticImg::make(glm::vec2(0.0f), images.getImg(img::ImgID::Compass240), 2));
    }
    else if (self->compass->angle > 135 && self->compass->angle <= 165) {
        compassFlex->push(widget::StaticImg::make(glm::vec2(0.0f), images.getImg(img::ImgID::Compass210), 2));
    }
    else if (self->compass->angle > 165 && self->compass->angle <= 195) {
        compassFlex->push(widget::StaticImg::make(glm::vec2(0.0f), images.getImg(img::ImgID::Compass180), 2));
    }
    else if (self->compass->angle > 195 && self->compass->angle <= 225) {
        compassFlex->push(widget::StaticImg::make(glm::vec2(0.0f), images.getImg(img::ImgID::Compass150), 2));
    }
    else if (self->compass->angle > 225 && self->compass->angle <= 255) {
        compassFlex->push(widget::StaticImg::make(glm::vec2(0.0f), images.getImg(img::ImgID::Compass120), 2));
    }
    else if (self->compass->angle > 255 && self->compass->angle <= 285) {
        compassFlex->push(widget::StaticImg::make(glm::vec2(0.0f), images.getImg(img::ImgID::Compass90), 2));
    } 
    else if (self->compass->angle > 285 && self->compass->angle <= 315) {
        compassFlex->push(widget::StaticImg::make(glm::vec2(0.0f), images.getImg(img::ImgID::Compass60), 2));
    }
    else {
        compassFlex->push(widget::StaticImg::make(glm::vec2(0.0f), images.getImg(img::ImgID::Compass30), 2));
    }
    this->addWidget(std::move(compassFlex));

    auto needleFlex = widget::Flexbox::make(
        glm::vec2(WINDOW_WIDTH - font::getRelativePixelsHorizontal(700), compassHeight),
        glm::vec2(0.0f, 0.0f),
        widget::Flexbox::Options(widget::Dir::VERTICAL, widget::Align::LEFT, font::getRelativePixels(5))
    );
    needleFlex->push(widget::StaticImg::make(glm::vec2(0.0f), images.getImg(img::ImgID::Needle), 2));

    if (orb_pos.has_value()) {
        auto player_pos_ground = self->physics.corner;
        player_pos_ground.y = 0;

        auto distance = glm::distance(orb_pos.value(), player_pos_ground);

        std::stringstream ss;

        ss << std::fixed << std::setprecision(1) << "  " << distance << "m";

        const float MAX_DIST = 150.0f;
        float dist_frac = std::max(std::min(distance / MAX_DIST, 1.0f), 0.0f);

        glm::vec3 close_color = font::getRGB(font::Color::GREEN);
        glm::vec3 far_color = font::getRGB(font::Color::RED);

        glm::vec3 color = (dist_frac * far_color) + ((1 - dist_frac) * close_color);

        needleFlex->push(widget::DynText::make(ss.str(), fonts,
            widget::DynText::Options(font::Font::TEXT, font::Size::SMALL, color))
        );
    }
    this->addWidget(std::move(needleFlex));

    //  Show a large splash text at the center of the screen for the player if the player
    //  successfully reflected a lightning bolt
    if (self.get().playerInfo.get().used_mirror_to_reflect_lightning) {
        std::cout << "Player used a mirror to reflect a lightning bolt!" << std::endl;
        auto reflectedLightningSplashText = widget::CenterText::make(
            "Reflected Lightning using Mirror!",
            gui::font::Font::TITLE,
            gui::font::Size::MEDIUM,
            gui::font::Color::WHITE,
            this->fonts,
            WINDOW_HEIGHT / 2
        );

        this->addWidget(std::move(reflectedLightningSplashText));
    }
}

void GUI::_layoutGameEscMenu() {
    auto self_eid = client->session->getInfo().client_eid;
    auto is_dm = client->session->getInfo().is_dungeon_master;

    auto exitBG = widget::Flexbox::make(
        glm::vec2(font::getRelativePixels(2), FRAC_WINDOW_HEIGHT(1, 2) - font::getRelativePixels(15)),
        glm::vec2(WINDOW_WIDTH, 0.0f),
        widget::Flexbox::Options(widget::Dir::VERTICAL, widget::Align::CENTER, 0.0f)
    );
    if(!is_dm.value()){
        exitBG->push(widget::StaticImg::make(glm::vec2(0.0f), images.getImg(img::ImgID::ExitBG), 2));
    } else {
        exitBG->push(widget::StaticImg::make(glm::vec2(0.0f), images.getImg(img::ImgID::ExitDMBG), 2));
    }

    exitBG->addOnClick([this](widget::Handle handle) {
        glfwSetWindowShouldClose(this->client->getWindow(), GL_TRUE);
    });

    this->addWidget(std::move(exitBG));

    auto exit_game_txt = widget::DynText::make(
        "Exit Game",
        fonts,
        widget::DynText::Options(font::Font::MENU, font::Size::MEDIUM, font::Color::WHITE)
    );
    exit_game_txt->addOnHover([this](widget::Handle handle) {
        auto widget = this->borrowWidget<widget::DynText>(handle);
        widget->changeColor(font::Color::YELLOW);
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

    std::string result_string = won ? "Victory" : "Defeat";

    auto victoryBG = widget::Flexbox::make(
        glm::vec2(font::getRelativePixels(2), FRAC_WINDOW_HEIGHT(1, 2) - font::getRelativePixels(55)),
        glm::vec2(WINDOW_WIDTH, 0.0f),
        widget::Flexbox::Options(widget::Dir::VERTICAL, widget::Align::CENTER, 0.0f)
    );

    if (won) {
        victoryBG->push(widget::StaticImg::make(glm::vec2(0.0f), images.getImg(img::ImgID::Victory), 2));
    }
    else {
        victoryBG->push(widget::StaticImg::make(glm::vec2(0.0f), images.getImg(img::ImgID::Defeat), 2));
    }

    this->addWidget(std::move(victoryBG));

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
