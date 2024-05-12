#pragma once

#include "server/game/item.hpp"
#include "server/game/object.hpp"
#include <chrono>

enum class PotionType {
	Health,
    Nausea,
    Invisibility
};

class Potion : public Item {
public:
    Potion(glm::vec3 corner, glm::vec3 dimensions, PotionType type);

    int duration; 
    int effectScalar;
    PotionType potType;

    void useItem(Object* other, ServerGameState& state) override;
    bool timeOut();
    void revertEffect(ServerGameState& state);

private:
    std::chrono::time_point<std::chrono::system_clock> used_time;
    Player* usedPlayer;
};