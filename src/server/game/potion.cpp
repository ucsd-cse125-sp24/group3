#pragma once

#include "server/game/potion.hpp"
#include "server/game/servergamestate.hpp"
#include "server/game/objectmanager.hpp"
#include "server/game/object.hpp"

Potion::Potion(): 
    Item(ObjectType::Potion)
{
    this->duration = 0;
    this->effectScalar = 0;
    this->potType = PotionType::Health;
    this->iteminfo.type = ItemType::Potion;
}

void Potion::setDuration(int duration) {
    this->duration = duration;
}

void Potion::seteffectScalar(int scalar) {
    this->effectScalar = scalar;
}

void Potion::setPotionType(PotionType type) {
    this->potType = type;
}