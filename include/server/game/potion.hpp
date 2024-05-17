#pragma once

#include "server/game/item.hpp"
#include "server/game/object.hpp"
#include <chrono>

/*
 *  Different types of potions
 */
enum class PotionType {
	Health,
    Nausea,
    Invisibility,
    Invincibility
};

class Potion : public Item {
public:
    /**
     * @param corner     Corner position of the Potion
     * @param dimensions Dimensions applied for the Potion
     * @param type       Type of Potion
     */
    Potion(glm::vec3 corner, glm::vec3 dimensions, PotionType type);

    int duration; 
    int effectScalar;
    PotionType potType;

    void useItem(Object* other, ServerGameState& state, int itemSelected) override;
    bool timeOut();
    void revertEffect(ServerGameState& state);

private:
    std::chrono::time_point<std::chrono::system_clock> used_time;
    Player* usedPlayer;
};