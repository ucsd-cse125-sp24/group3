#pragma once

// #include "client/core.hpp"

#include <string>
#include <functional>
#include <vector>

namespace gui {

using WidgetCallback = std::function<void()>;

class Widget {
    public:
        Widget(std::string asset_path);

        Widget& place(float r_x, float r_y);
        Widget& onClick(WidgetCallback callback);
        Widget& onHover(WidgetCallback callback);

        virtual void render() = 0;

    private:
        std::vector<WidgetCallback> onClicks;
        std::vector<WidgetCallback> onHovers;

        /// @brief relative x position on screen from -1 -> 1
        float r_x { 0.0f };
        /// @brief relative y position on screen from -1 -> 1
        float r_y { 0.0f };

        /// @brief filepath to image for the widget
        std::string asset_path;
};

}
