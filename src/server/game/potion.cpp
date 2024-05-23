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
        this->effectScalar = 0;
        this->modelType = ModelType::InvisibilityPotion;
        break;
    case PotionType::Invincibility:
        this->duration = INVINCIBLITY_DUR;
        this->effectScalar = INVINCIBLITY_SCALAR;
        this->modelType = ModelType::InvincibilityPotion;
        break;
    }
}

void Potion::useItem(Object* other, ServerGameState& state, int itemSelected) {
    auto player = dynamic_cast<Player*>(other);
    this->usedPlayer = player;

    this->used_time = std::chrono::system_clock::now();
    auto now = std::chrono::system_clock::now();
    std::chrono::duration<double> elapsed_seconds{ this->used_time - now };
    this->iteminfo.remaining_time = (double)this->duration - elapsed_seconds.count();

    // add onto current effect if it exists
    for (const auto& [id, value] : player->sharedInventory.usedItems) {
        if (value.first != this->modelType) {
            continue;
        }
        auto currentPotion = dynamic_cast<Potion*>(state.objects.getItem(id));

        currentPotion->duration += this->duration;
        currentPotion->iteminfo.remaining_time += this->duration;

        this->iteminfo.used = true;
        this->iteminfo.held = false;

        Item::useItem(other, state, itemSelected);
        state.markForDeletion(this->globalID);
        return;
    }


    // if new potion effect
    switch (this->potType) {
    case PotionType::Health: {
        player->stats.health.increase(this->effectScalar);
        break;
    }
    case PotionType::Nausea: {
        player->physics.nauseous = this->effectScalar;
        player->sharedInventory.usedItems.insert({ this->typeID, std::make_pair(ModelType::NauseaPotion, this->iteminfo.remaining_time) });
        break;
    }
    case PotionType::Invisibility: {
        player->info.render = false;
        player->sharedInventory.usedItems.insert({ this->typeID, std::make_pair(ModelType::InvisibilityPotion, this->iteminfo.remaining_time) });
        break;
    }
    case PotionType::Invincibility: {
        player->stats.health.addMod(this->effectScalar);
        player->sharedInventory.usedItems.insert({ this->typeID, std::make_pair(ModelType::InvincibilityPotion, this->iteminfo.remaining_time) });
        break;
    }
    }

    this->iteminfo.used = true;
    this->iteminfo.held = false;

    Item::useItem(other, state, itemSelected);
}

bool Potion::timeOut() {
    auto now = std::chrono::system_clock::now();
    std::chrono::duration<double> elapsed_seconds{ now - this->used_time };
    this->iteminfo.remaining_time = (double)this->duration - elapsed_seconds.count();

    if (this->usedPlayer->sharedInventory.usedItems.find(this->typeID) != this->usedPlayer->sharedInventory.usedItems.end()) {
        this->usedPlayer->sharedInventory.usedItems[this->typeID].second = this->iteminfo.remaining_time;
    }
    return (now - this->used_time) > std::chrono::seconds(this->duration);
}

UsedItemsMap::iterator Potion::revertEffect(ServerGameState& state) {
    switch (this->potType) {
    case PotionType::Nausea: {
        this->usedPlayer->physics.nauseous = 1.0f;
        break;
    }
    case PotionType::Invisibility: {
        this->usedPlayer->info.render = true;
        break;
    }
    case PotionType::Invincibility: {
        this->usedPlayer->stats.health.subMod(this->effectScalar);
        this->usedPlayer->stats.health.increase(this->effectScalar);
        break;
    }                          
    }
    state.markForDeletion(this->globalID);

    auto it = this->usedPlayer->sharedInventory.usedItems.find(this->typeID);
    if (it != this->usedPlayer->sharedInventory.usedItems.end()) {
        it = this->usedPlayer->sharedInventory.usedItems.erase(it);
    }
    return it;
}