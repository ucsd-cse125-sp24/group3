#include "client/gui/gui.hpp"

#include <iostream>

namespace gui {

    bool GUI::init() {
        std::cout << "Initializing GUI...\n";

        if (!this->fonts.init()) {
            return false;
        }

        std::cout << "Initialized GUI\n";
        return true;
    }

}