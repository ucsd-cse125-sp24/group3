#pragma once

// Include all gui headers so everyone else just needs to include this file
#include "client/gui/widget/options.hpp"
#include "client/gui/widget/type.hpp"
#include "client/gui/widget/widget.hpp"
#include "client/gui/widget/dyntext.hpp"
#include "client/gui/widget/flexbox.hpp"
#include "client/gui/widget/staticimg.hpp"
#include "client/gui/widget/centertext.hpp"
#include "client/gui/widget/textinput.hpp"
#include "client/gui/widget/empty.hpp"
#include "client/gui/font/font.hpp"
#include "client/gui/font/loader.hpp"
#include "client/gui/img/img.hpp"
#include "client/gui/img/loader.hpp"
#include "server/game/constants.hpp"
#include "client/gui/img/logo.hpp"
#include "shared/utilities/config.hpp"

#include <iostream>
#include <vector>
#include <memory>
#include <map>
#include <boost/optional/optional.hpp>

//  Forward declarration of LobbyPlayer struct
struct LobbyPlayer;

class Client;

namespace gui {

/**
 * Enumeration for all of the different "screens" that can be rendered by the GUI. The GUI class
 * itself doesn't contain any internal state related to any specific state, but instead takes
 * a GUIState as input to determine what should be rendered to the screen.
 */
enum class GUIState {
    NONE,
    INITIAL_LOAD,
    TITLE_SCREEN,
    LOBBY_BROWSER,
    LOBBY,
    INTRO_CUTSCENE,
    GAME_HUD,
    GAME_ESC_MENU,
    DEAD_SCREEN,
    RESULTS_SCREEN
};

#define GUI_PROJECTION_MATRIX() glm::ortho(0.0f, (float)WINDOW_WIDTH, 0.0f, (float)WINDOW_HEIGHT);

/**
 * Class which wraps around all of the GUI elements that should be rendered to the screen.
 * 
 * The GUI can essentially be thought of as a collection of "Widgets" which all have "Handles" (or IDs).
 * Each widget has all of the logic it needs to know its size, location, and how to render it.
 * The GUI class acts as a container for all of these widgets and provides a helpful abstraction layer
 * allowing the rest of the code to think in terms of "GUIState", since you can just tell the GUI
 * to render a specific state, and it will do so.
 */
class GUI {
public:
    /// =<SETUP>========================================================================
    ///
    /// These are the functions that need to be called to setup a GUI object. Doing anything
    /// else before calling these will probably cause a crash.
    ///
    /**
     * @brief Stores the client pointer as a data member.
     * 
     * Constructor for a GUI. This really does nothing except store a pointer to the Client object
     * so that the GUI functions can easily access Client data members, due to the friend relationship
     * of Client -> GUI.
     * 
     * @param client Pointer to the client object. Note that GUI is a friend class to client, so GUI can
     * access private client data members for ease of use.
     */
    explicit GUI(Client* client, const GameConfig& config);
    /**
     * @brief Initializes all of the necessary file loading for all of the GUI elements, and
     * registers all of the static shader variables for each of the derived widget classes
     * that need a shader.
     */
    bool init();
    /// ================================================================================

    /// =<RENDERING>====================================================================
    ///
    /// These series of functions are what you actually use to do the displaying and
    /// rendering of the GUI. They should be called in the order they are listed in
    /// this file. 
    ///
    /**
     * @brief Wipes all of the previous frame's state
     * 
     * Function that should be called at the beginning of a frame. Essentially it wipes
     * all of the previous frame's widget data.
     */
    void beginFrame();
    /**
     * @brief Adds widgets to the layout depending on the specified GUI state.
     * 
     * @param state Current State of the GUI that should be rendered. This essentially
     * corresponds to a specific "screen" that should be displayed
     */
    void layoutFrame(GUIState state);
    /**
     * @brief Takes the current relevant input information and alters the GUI based on the
     * how the inputs interact with all of the handlers assigned to the Widgets.
     * 
     * NOTE: this must be called after adding all of the widgets to the screen, otherwise
     * no event handling will work on those widgets added after calling this function.
     * 
     * NOTE: currently this function takes a reference to the mouse down boolean because
     * if it "captures" a mouse click then it sets that boolean to false, to prevent continued
     * event triggers if the mouse is held down. In other words, we want "onClick" events
     * to happen only on the initial click, not continually while the mouse is held down.
     * 
     * NOTE: both of the x and y coordinates of the mouse passed into this function are in
     * the GLFW coordinate space. In GLFW coordinates, the top left corner of the screen is
     * (x=0,y=0). However, in the GUI world all of our coordinates are based off of 
     * (x=0,y=0) being in the bottom left corner. This is the only GUI function that takes
     * GLFW coordinates, 
     * 
     * @param mouse_xpos x position of the mouse, in GLFW Coordinates
     * @param mouse_ypos y position of the mouse, in GLFW Coordinates
     * @param is_left_mouse_down reference to flag which stores whether or not the left mouse
     * is down. Note: this is a reference so that if a click is captured it can toggle the
     * mouse down flag to false, so that click doesn't get "double counted" in subsequent
     * frames.
     */
    void handleInputs(float mouse_xpos, float mouse_ypos, bool& is_left_mouse_down);
    /**
     * Renders the current state of the GUI to the screen, based on all of the widgets
     * that have been added and any changes caused by inputs.
     */
    void renderFrame();
    /// ==============================================================================

    /// =<WIDGET MANIPULATION>========================================================
    ///
    /// These functions are concerned with adding, removing, and getting widgets to/from
    /// the GUI.
    ///
    /**
     * @brief Adds the specified widget to the GUI.
     * 
     * NOTE: the widget that is being passed in is of the type Widget::Ptr, which is
     * a typedef for a std::unique_ptr<Widget>. To easily get a unique_ptr for a Widget,
     * you can use the corresponding static `make` function provided by each widget
     * implementation.
     * 
     * @param widget Widget to add
     * @returns A handle to the widget that was added, so it can be modifed/removed later
     */
    widget::Handle addWidget(widget::Widget::Ptr&& widget);
    /**
     * @brief Removes the specified widget from the GUI
     * 
     * @param handle Handle to the widget that you want to remove
     * @returns The widget that you removed, now isolated from the GUI.
     */
    widget::Widget::Ptr removeWidget(widget::Handle handle);
    /**
     * @brief Borrows the specified widget from the GUI.
     * This is especially useful inside of callback functions for widgets as you will 
     * already have the handle needed to initiate a borrow.
     * 
     * NOTE: This function essentially returns the raw pointer for the specified widget.
     * Technically you could do unspeakable things to this pointer, but as we are all
     * bound by societal conventions so shall you too not break this taboo. 
     * 
     * NOTE: You should not attempt to save this pointer elsewhere. This pointer can be thought
     * of as a "temporary" reference to the Widget, which will go out of scope
     * between the time of calling and the next frame.
     * 
     * NOTE: The template argument you specify essentially performs a dynamic cast on the
     * returned pointer. You should not make your template argument a pointer itself, since
     * the function return type already adds a pointer to it. You should only specify a
     * specific kind of Widget if you are confident in what kind of widget it is (i.e. in
     * an event handler). Otherwise, it is safe to just specify a widget::Widget as the
     * template argument
     * 
     * Example use:
     *     // Precondition: We know that `handle` is a valid handle to a widget::DynText
     *     auto widget = gui.borrowWidget<widget::DynText>(handle);
     *     // widget is of type `widget::DynText*`
     * 
     * If you mess up and pass in the wrong template argument, then the pointer will
     * end up being nullptr.
     * 
     * @tparam Type of the widget that you are trying to borrow.
     * @param handle Handle to the widget you want to borrow
     * @returns Pointer to the widget specified by the handle, casted to
     * be a pointer of the specified template type.
     */
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
    /// ==============================================================================

    /// =<KEYBOARD INPUT>=============================================================
    ///
    /// These functions are concerned with keyboard input for the TextInput widget.
    /// Because of the way this is implemented, there can only be one TextInput widget
    /// on the screen at one time. If there are more, they will end up sharing all of
    /// the inputted text.
    ///
    /**
     * @brief Checks to see if the GUI is currently capturing keyboard input.
     * 
     * NOTE: This shouldn't be called as a precondition for `captureKeystroke` because 
     * `captureKeystroke` will internally also check whether or not the GUI is
     * capturing keystrokes.
     * 
     * @returns true if the GUI is capturing keyboard input, false otherwise
     */
    bool shouldCaptureKeystrokes() const;
    /**
     * @brief Toggles whether or not the GUI should be capturing keyboard input
     * 
     * @param should_capture Whether or not the GUI should be capturing keyboard input
     */
    void setCaptureKeystrokes(bool should_capture);
    /**
     * @brief Captures a keystroke as an ASCII char.
     * 
     * Takes a keystroke as captured by the client's GLFW code, and adds it into the GUI.
     * Internally, this will check that the ASCII value is between [32, 126], which is the
     * set of meaningful ASCII values that we care about.
     * 
     * NOTE: this function internally checks whether or not the GUI is capturing keyboard
     * input. If the GUI is not currently capturing keyboard input, then this will do
     * nothing. This means that you don't need to check `shouldCaptureKeystrokes` before
     * calling this function.
     * 
     * NOTE: This function does not handle backspaces. To handle backspaces, the
     * `captureBackspace` function should be used instead.
     * 
     * @param c ASCII char to capture
     */
    void captureKeystroke(char c);
    /**
     * @brief If the GUI is capturing backspaces, then records a backspace press. This deletes
     * the most recently captured keystroke in the GUI.
     */
    void captureBackspace();
    /**
     * @brief Wipes all of the captured keyboard input from the internal state.
     */
    void clearCapturedKeyboardInput();
    /**
     * @brief Displays controls for the player
     */
    void displayControl();
    /**
     * @brief Returns all of the captured keyboard input without clearing it. 
     * 
     * @returns all of the captured keyboard input as an std::string
     */
    std::string getCapturedKeyboardInput() const;
    /// ==============================================================================

    /// =<GETTERS>====================================================================
    ///
    /// Getters for various private data members
    ///
    /**
     * @brief Getter for the font loader
     * @returns shared pointer to the font loader
     */
    std::shared_ptr<font::Loader> getFonts();
    /// ==============================================================================

private:
    widget::Handle next_handle {0};
    std::map<widget::Handle, widget::Widget::Ptr> widgets;

    std::shared_ptr<font::Loader> fonts;
    img::Loader images;

    bool capture_keystrokes;
    std::string keyboard_input;

    Client* client;

    GameConfig config;
    bool controlDisplayed;

    std::vector<std::string> recentEvents;

    img::Logo logo;

    /// =<INTERNAL HELPERS>==========================================================
    /**
     * @brief Performs a click on the specied coordinate in the GUI coordinate frame.
     * 
     * This is what will call the click callback functions on the widgets.
     * 
     * @param x x coordinate of click in GUI coordinates
     * @param y y coordinate of click in GUI coordinates
     */
    void _handleClick(float x, float y);
    /**
     * @brief Performs a mouse hover on the specied coordinate in the GUI coordinate frame.
     * 
     * This is what will call the hover callback functions on the widgets.
     * 
     * @param x x coordinate of click in GUI coordinates
     * @param y y coordinate of click in GUI coordinates
     */
    void _handleHover(float x, float y);
    /// =============================================================================

    /// =<GUI LAYOUTS>===============================================================
    ///
    /// These are all of the internal helper functions which render a specified GUIState
    /// layout. These are where all of the widget manipulation should occur.
    ///
    /// NOTE: The widget manipulation functions are public, so you can also do further
    /// widget manipulation outside of one of these functions. However, if you feel the
    /// need to do this you should consider whether or not what you're doing makes more
    /// sense to encode as a GUIState. The reason those functions are public are to do
    /// more fine tuned GUI manipulation which may only make sense to do outside of these
    /// preset "layouts".
    ///
    /**
     * @brief Displays a loading screen while the game is starting up
     */
    void _layoutLoadingScreen();
    /**
     * @brief Displays the title screen layout
     * 
     * Transitions to the LobbyBrowser once "Start Game" is clicked.
     */
    void _layoutTitleScreen();
    /**
     * @brief Displays the lobby browser layout
     * 
     * Allows the user to input a name.
     * Transitions to the Lobby once a lobby is selected to join.
     */
    void _layoutLobbyBrowser();

    /**
     * @brief Display fps counter on the top left corner
     */
    void _layoutFPSCounter();

    /**
     * @brief Displays the lobby layout
     * 
     * Does not provide any GUI-initiated transitions, as it instead waits for the
     * server to specify that the game has started with a LoadGameStateEvent
     * 
     * Displays the lobby name and all of the players who are currently in the lobby.
     */
    void _layoutLobby();

    /**
     * @brief Creates a player status row (which is represented as a Flexbox widget) for the
     * given player. This function generates the player status row such that the 3 columns
     * have the specified width (this is done using the Empty widget).
     * @param lobbyPlayer Player for whom the player status row is created.
     * @param columnWidths Widths for the 3 columns of the player status row.
     * @param origin Origin for this player status row's Flexbox.
     * @param playerIndex 1-indexed player index for the given LobbyPlayer.
     * @return gui::widget::Flexbox::Ptr of a Flexbox storing the generated player status row.
    */
    gui::widget::Flexbox::Ptr _createPlayerStatusRow(boost::optional<LobbyPlayer> lobbyPlayer,
        glm::vec3 columnWidths, glm::vec2 origin, int playerIndex);
    /**
     * @brief Displays the Game HUD layout
     * 
     * TODO: this is not implemented yet
     */
    void _layoutGameHUD();

    /**
    * @brief Displays the Game HUD for both gameHUD and EscMenu layout
    */
    void _sharedGameHUD();

    /**
     * @brief Displays the menu which appears when the player presses Escape while playing
     * 
     * Known bugs:
     * BUG: The game stops rendering the game when this is being displayed
     * BUG: Mouse movement is still tracked causing disorienting reorientation upon resuming
     */
    void _layoutGameEscMenu();
    /**
     * @brief Displays the screen which appears when the player fucking DIES
     */
    void _layoutDeadScreen();

    /**
     * @brief Displays the screen which appears when either player group wins
     * the game.
     */
    void _layoutResultsScreen();

    /// =============================================================================
};

}
