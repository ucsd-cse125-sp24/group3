#ifndef SHARED_GAME_GAMESTATE_HPP_
#define SHARED_GAME_GAMESTATE_HPP_

#include "shared/utilities/serializable.hpp"

#include <string>

class GameState : public Serializable {
public:
    GameState();

    std::string serialize() const override; 
private:

};

#endif  // SHARED_GAMESTATE_HPP_