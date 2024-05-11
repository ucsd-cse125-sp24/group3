#pragma once

#include "server/game/potion.hpp"
#include "shared/game/sharedobject.hpp"
#include "server/game/servergamestate.hpp"
#include "server/game/objectmanager.hpp"

Potion::Potion(): 
    Item(ObjectType::Potion)
{
    this->duration = 0;
    this->effectScalar = 0;
    this->potType = PotionType::Health;
    this->physics.collider = Collider::Box;
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
    switch (type) {
    case PotionType::Health: {
        this->setModel(ModelType::HealthPotion);
        break;
    }
    case PotionType::Swiftness: {
        this->setModel(ModelType::SwiftnessPotion);
        break;
    }
    case PotionType::Invisibility: {
        this->setModel(ModelType::InvisibilityPotion);
        break;
    }
    }
    
}