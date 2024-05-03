#pragma once

#include "client/core.hpp"

#include <cstddef>
#include <string>

namespace gui::font {

/**
 * Abstract representation of the different fonts to use in our game
 * 
 * NOTE: currently I haven't found a good font for "Text", so both of these
 * map to the same font.
 */
enum class Font {
    MENU,
    TEXT,
};

/**
 * Preset sizes for fonts
 */
enum FontSizePx {
    SMALL = 64,
    MEDIUM = 96,
    LARGE = 128,
    BIG_YOSHI = 256 // https://www.google.com/search?q=big+yoshi&sca_esv=f28e476185e14b90&udm=2&biw=1504&bih=927&sxsrf=ACQVn082--gJOfKGRnYyCSUnjdkNWOUwIg%3A1714695676102&ei=_C00ZrvlBdDWkPIP3bGh6Ac&ved=0ahUKEwi706-Vm_CFAxVQK0QIHd1YCH0Q4dUDCBA&uact=5&oq=big+yoshi&gs_lp=Egxnd3Mtd2l6LXNlcnAiCWJpZyB5b3NoaTIEECMYJzIFEAAYgAQyBRAAGIAEMgUQABiABDIFEAAYgAQyBRAAGIAEMgUQABiABDIFEAAYgAQyBRAAGIAEMgUQABiABEj-BFDdAljSA3ACeACQAQCYATagAWaqAQEyuAEDyAEA-AEBmAIEoAJ5wgIKEAAYgAQYQxiKBZgDAIgGAZIHATSgB5MM&sclient=gws-wiz-serp
};

/**
 * Preset colors for text
 */
enum class FontColor {
    BLACK,
    RED,
    BLUE,
    GRAY
};

/**
 * Mappings from our specified abstract fonts to the file to load
 */
std::string getFilepath(Font font);

/**
 * Mapping from preset font colors to RGB values
 */
glm::vec3 getRGB(FontColor color);

}
