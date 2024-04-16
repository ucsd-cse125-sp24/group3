#pragma once

#include "shared/utilities/serialize_macro.hpp"

#include <string>

enum class GamePhase {
    TITLE_SCREEN,
    LOBBY,
    GAME
};

class GameState {
public:
    GameState() = default;

    DEF_SERIALIZE(Archive& ar, const unsigned int version) {
        ar & phase;
    }

private:
    GamePhase phase;
};
