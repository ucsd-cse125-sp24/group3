#pragma once

#include "server/game/potion.hpp"
#include "server/game/constants.hpp"
#include "shared/game/sharedobject.hpp"
#include <chrono>

class Player;

Potion::Potion(glm::vec3 corner, glm::vec3 dimensions, PotionType type):
    Item(ObjectType::Potion, false, corner, ModelType::Cube, dimensions)
{   
    this->duration = 0;
    this->effectScalar = 0;
    this->potType = type;
    this->usedPlayer = nullptr;

    switch (type) {
    case PotionType::Health:
        this->duration = HEALTH_DURATION;
        this->effectScalar = RESTORE_HEALTH;
        this->modelType = ModelType::HealthPotion;
        break;
    case PotionType::Nausea:
        this->duration = NAUSEA_DURATION;
        this->effectScalar = NAUSEA_SCALAR;
        this->modelType = ModelType::NauseaPotion;
        break;
    case PotionType::Invisibility:
        this->duration = INVIS_DURATION;
        this->effectScalar = INVIS_OPACITY;
        this->modelType = ModelType::InvisibilityPotion;
        break;
    }
}

void Potion::useItem(Object* other, ServerGameState& state) {
    Player* player = dynamic_cast<Player*>(other);
    this->usedPlayer = player;

    this->used_time = std::chrono::system_clock::now();

    switch (this->potType) {
    case PotionType::Health: {
        player->stats.health.increase(this->effectScalar);
        break;
    }
    case PotionType::Nausea: {
        player->physics.nauseous = this->effectScalar;
        break;
    }
    case PotionType::Invisibility: {

        break;
    }
    }

    this->iteminfo.used = true;
    this->iteminfo.held = false;
}

bool Potion::timeOut() {
    auto now = std::chrono::system_clock::now();
    return (now - this->used_time) > std::chrono::seconds(this->duration);
}

void Potion::revertEffect(ServerGameState& state) {
    switch (this->potType) {
    case PotionType::Nausea: {
        this->usedPlayer->physics.nauseous = 1.0f;
        break;
    }
    case PotionType::Invisibility: {

        break;
    }
    }

    state.markForDeletion(this->globalID);
}