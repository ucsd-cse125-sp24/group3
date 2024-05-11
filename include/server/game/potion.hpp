#pragma once

#include "server/game/item.hpp"
#include "server/game/object.hpp"
#include "server/game/servergamestate.hpp"

enum class PotionType {
	Health,
	Swiftness,
    Invisibility
};

class Potion : public Item {
public:
    Potion();

    int duration; 
    int effectScalar;
    PotionType potType;

    void setDuration(int duration);
    void seteffectScalar(int scalar);
    void setPotionType(PotionType type);

private:

};