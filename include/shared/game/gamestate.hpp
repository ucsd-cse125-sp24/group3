#pragma once

#include "shared/utilities/serializable.hpp"

#include <string>

class GameState : public Serializable {
public:
    GameState() = default;

    /**
     * Serializes GameState instances to be sent over the
     * network.
     * @return serialized string of GameState 
     */
    [[nodiscard]] std::string serialize() const override; 
private:

};
