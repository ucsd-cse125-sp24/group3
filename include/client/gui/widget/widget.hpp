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
     * Renders the widget to the screen using the specified shader
     */
    virtual void render() = 0;
    /// ======================================================================================


    [[nodiscard]] Type getType() const;
    [[nodiscard]] const glm::vec2& getOrigin() const;
    [[nodiscard]] std::pair<std::size_t, std::size_t> getSize() const;
    [[nodiscard]] Handle getHandle() const;
    [[nodiscard]] virtual bool hasHandle(Handle handle) const;
    [[nodiscard]] virtual Widget* borrow(Handle handle);

protected:
    Handle handle;
    static std::size_t num_widgets;

    Type type;
    glm::vec2 origin;
    std::size_t width  {0};
    std::size_t height {0};

    std::unordered_map<CallbackHandle, Callback> on_clicks;
    std::unordered_map<CallbackHandle, Callback> on_hovers;

private:
    CallbackHandle next_click_handle {0};
    CallbackHandle next_hover_handle {0};

    bool _doesIntersect(float x, float y) const;
};


}
