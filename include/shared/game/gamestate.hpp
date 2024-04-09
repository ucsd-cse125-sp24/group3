#pragma once

#include "shared/utilities/serializable.hpp"

#include <string>

class GameState : public Serializable {
public:
    GameState();

    std::string serialize() const override; 
private:

};
