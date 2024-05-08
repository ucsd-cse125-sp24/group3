#pragma once

#include "client/core.hpp"
#include "client/gui/widget/type.hpp"
#include "client/gui/widget/options.hpp"

#include <memory>
#include <unordered_set>
#include <string>
#include <functional>
#include <unordered_map>

namespace gui::widget {

/// @brief Type for a Widget Handle
using Handle = std::size_t;
/// @brief Type for the handle for a specific callback function
using CallbackHandle = std::size_t;
/// @brief Type for a onClick or onHover callback function.
/// The Handle parameter is the handle to the widget that the callback is
/// attached to.
using Callback = std::function<void(Handle)>;

/**
 * Abstract base class for any arbitrary widget.
 * 
 * Any Widget implementation must override the following pure virtual functions:
 *     - render
 * 
 * And certain widget implementations may also wish to override these virtual functions
 *     - doClick
 *     - doHover
 *     - hasHandle
 *     - borrow 
 * 
 * You can see the documentation for these functions for explanations of why you may or
 * may not need to override this functions for a specific derived widget.
 * 
 * In addition, any derived class must also set width and height once these values are known.
 */
class Widget {
public:
    /// All widgets are passed around and manipulated as unique ptrs so this is a helpful
    /// alias to reduce characters typed
    using Ptr = std::unique_ptr<Widget>;

    /// =<SETUP>==============================================================================
    ///
    /// These functions are necessary to setup and position a widget on the screen.
    ///
    /**
     * @brief Sets the type and origin position of the widget, and assigns a new unique handle.
     * 
     * @param type Type of the widget
     * @param origin Origin position of the widget (bottom left x,y coordinate in the GUI
     * coordinate frame)
     */
    explicit Widget(Type type, glm::vec2 origin);
    /**
     * @brief Overrides the current origin position with the newly specified one.
     * 
     * NOTE: This is helpful when you may not know the exact origin position of a widget at
     * construction time (i.e. when inserting a widget inside of a flexbox, as the flexbox)
     * then becomes responsible for positioning the widget).
     * 
     * @param origin Bottom left (x,y) coordinate of the widget in the GUI coordinate frame.
     */
    void setOrigin(glm::vec2 origin);
    /// ======================================================================================

    /// =<ACTION HANDLING>====================================================================
    ///
    /// These functions are used to register, remove, and activate action callbacks on widgets
    /// 
    /**
     * @brief Register a new onClick callback function for this widget.
     * 
     * @param callback Callback function to run on click
     * 
     * @returns handle to the callback function, which can be passed into removeOnClick to
     * unregister.
     */
    CallbackHandle addOnClick(Callback callback);
    /**
     * @brief Register a new onHover callback function for this widget.
     * 
     * @param callback Callback function to run on hover
     * 
     * @returns handle to the callback function, which can be passed into removeOnHover to
     * unregister.
     */
    CallbackHandle addOnHover(Callback callback);
    /**
     * @brief Removes the onClick callback function associated with the given handle
     * 
     * @param handle Handle to the onClick callback function you want to remove
     */
    void removeOnClick(CallbackHandle handle);
    /**
     * @brief Removes the onHover callback function associated with the given handle
     * 
     * @param handle Handle to the onHover callback function you want to remove
     */
    void removeOnHover(CallbackHandle handle);
    /**
     * @brief Performs a click at the specified (x,y) position in the GUI coordinate frame.
     * 
     * NOTE: This function's main role is to determine if a click at the specified (x,y)
     * coordinate should trigger the onClick handlers for this widget. The default implementation
     * of this checks to see if the (x,y) coordinate is within the bounds specified by `origin`
     * and `width` and `height`, and if so calls all of the onClick handlers.
     * 
     * NOTE: One reason for a derived class to override this function is if the widget itself
     * contains other widgets, because in that case doClick will not be called on those "subwidgets"
     * unless this function passes the call down the chain
     * 
     * @param x x coordinate of the click in GUI coordinates
     * @param y y coordinate of the click in GUI coordinates
     */
    virtual void doClick(float x, float y);
    /**
     * @brief Performs a hover action at the specified (x,y) position in the GUI coordinate frame
     * 
     * NOTE: see the documentation for `doClick`, as everything said there also applies here, but just
     * for the hover actions.
     */
    virtual void doHover(float x, float y);
    /// ======================================================================================

    /// =<DISPLAY>============================================================================
    /**
     * @brief Renders the widget to the screen
     * 
     * This is the only function that must be overridden by derived classes.
     */
    virtual void render() = 0;
    /// ======================================================================================

    /// =<SIMPLE GETTERS>=====================================================================
    /**
     * @brief Queries the widget::Type of the widget 
     * @return Type of the widget
     */
    [[nodiscard]] Type getType() const;
    /**
     * @brief Queries the origin position of the widget
     * @return origin position of the widget in GUI coordinates
     */
    [[nodiscard]] const glm::vec2& getOrigin() const;
    /**
     * @brief Queries the size of the widget in pixels
     * @return the width and height of the widget
     */
    [[nodiscard]] std::pair<std::size_t, std::size_t> getSize() const;
    /**
     * @brief Queries the handle for this specific widget
     * @return the handle for this widget
     */
    [[nodiscard]] Handle getHandle() const;
    /// ======================================================================================

    /// =<WIDGET BORROWING>===================================================================
    ///
    /// The following functions are incredibly important for GUI manipulation, especially in
    /// event handlers. They essentially define how outside users can gain internal access to
    /// widgets that are inside of the GUI
    ///
    /// hasHandle should always be called before calling borrow if you aren't sure whether or
    /// not the widget actually has the specified handle, because borrow will terminate the
    /// program if the specified widget does not have the handle.
    /// 
    /// By default hasHandle and borrow only check the widget itself to see if the handle matches.
    /// This means that if some derived Widget acts as a container for some set of internal
    /// subwidgets, then these functions should be overridden to provide access to those subwidgets.
    ///
    /**
     * @brief Checks to see whether or not this widget "has" the specified handle, whether
     * because the handle is this widgets handle, or because this widget internally contains
     * some subwidget that has that handle.
     * 
     * @param handle Handle of the widget for which you want to query
     * 
     * @returns True if this widget is or contains the specified widget, false otherwise
     */
    [[nodiscard]] virtual bool hasHandle(Handle handle) const;
    /**
     * @brief Gives a pointer to the subwidget specified by the handle
     * 
     * NOTE: if `handle` is not a valid handle for either (1) this widget or (2) any subwidgets,
     * then this function will terminate the program and print out an error message.
     * 
     * @returns a pointer to the widget specified by handle
     */
    [[nodiscard]] virtual Widget* borrow(Handle handle);
    /// ======================================================================================

protected:
    /// @brief Handle for this widget
    Handle handle;

    /// @brief Static counter for the number of widgets that have been created, so a new identifier
    /// can be assigned to each widget as they are created
    static std::size_t num_widgets;

    /// @brief Type of the widget
    Type type;

    /// @brief Origin position (bottom left) of the widget in GUI coordinates
    glm::vec2 origin;
    
    /// NOTE: Both the widget and the height of the widget are initialized to 0 and are not set
    /// anywhere inside of this base class. Instead, derived classes must set these values themselves
    /// once they are known.

    /// @brief Width of the widget, in pixels
    std::size_t width  {0};
    /// @brief Height of the widget, in pixels
    std::size_t height {0};

    /// @brief all of the onClick handlers, indexable by handle
    std::unordered_map<CallbackHandle, Callback> on_clicks;
    /// @brief all of the onHover handlers, indexable by handle
    std::unordered_map<CallbackHandle, Callback> on_hovers;

private:
    /// @brief internal counter to assign to the next onClick handler
    CallbackHandle next_click_handle {0};
    /// @brief internal counter to assign to the next onHover handler
    CallbackHandle next_hover_handle {0};

    /// @brief helper function to determine if 
    bool _doesIntersect(float x, float y) const;
};


}
