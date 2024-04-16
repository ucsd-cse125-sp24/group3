#pragma once

#include "shared/utilities/serialize_macro.hpp"

#include <string>

class GameState {
public:
    GameState() = default;

    DEF_SERIALIZE(Archive& ar, const unsigned int version) {

    }
private:

};
