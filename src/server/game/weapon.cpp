#pragma once

#include "server/game/weapon.hpp"
#include "server/game/item.hpp"
#include "server/game/constants.hpp"
#include "server/game/servergamestate.hpp"
#include "server/game/object.hpp"

Weapon::Weapon(glm::vec3 corner, glm::vec3 dimensions, WeaponType weaponType):
    Item(ObjectType::Weapon, false, corner, ModelType::Cube, dimensions)
{
    
    this->weaponType = weaponType;

    switch (weaponType) {
    case WeaponType::Katana:
        this->modelType = ModelType::Katana;
        break;
    case  WeaponType::Dagger:
        this->modelType = ModelType::Dagger;
        break;
    case  WeaponType::Hammer:
        this->modelType = ModelType::Hammer;
        break;
    }
}

void Weapon::useItem(Object* other, ServerGameState& state, int itemSelected) {
    Player* player = dynamic_cast<Player*>(other);

    Item::useItem(other, state, itemSelected);
}

void Weapon::doCollision(Object* other, ServerGameState& state) {

    auto player = dynamic_cast<Player*>(other);
    if (player == nullptr) return;

    for (int i = 0; i < player->inventory.size(); i++) {
        if (player->inventory[i] != -1) { continue; }

        player->inventory[i] = this->typeID;
        player->sharedInventory.inventory[i] = this->modelType;
        player->sharedInventory.usesRemaining[i] = this->castLimit;
        break;
    }
    this->iteminfo.held = true;
    this->physics.collider = Collider::None;
}