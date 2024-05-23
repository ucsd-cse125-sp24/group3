#pragma once

#include "server/game/weapon.hpp"
#include "server/game/weaponcollider.hpp"
#include "server/game/item.hpp"
#include "server/game/constants.hpp"
#include "server/game/servergamestate.hpp"
#include "server/game/object.hpp"

Weapon::Weapon(glm::vec3 corner, glm::vec3 dimensions, WeaponType weaponType):
    Item(ObjectType::Weapon, false, corner, ModelType::Cube, dimensions)
{
    this->weaponType = weaponType;
    this->resetAttack = true;

    switch (weaponType) {
    case WeaponType::Sword:
        this->modelType = ModelType::Sword;
        this->delay = SWORD_TOTAL;
        break;
    case WeaponType::Dagger:
        this->modelType = ModelType::Dagger;
        this->delay = DAGGER_TOTAL;
        break;
    case WeaponType::Hammer:
        this->modelType = ModelType::Hammer;
        this->delay = HAMMER_TOTAL;
        break;
    }
}

void Weapon::useItem(Object* other, ServerGameState& state, int itemSelected) {
    Player* player = dynamic_cast<Player*>(other);

    glm::vec3 attack_origin(
        player->physics.shared.getCenterPosition().x,
        0,
        player->physics.shared.getCenterPosition().z
    );

    if (this->resetAttack) {
        switch (weaponType) {
        case WeaponType::Sword:
            state.objects.createObject(new MediumAttack(player, attack_origin, player->physics.shared.facing));
            break;
        case WeaponType::Dagger:
            state.objects.createObject(new ShortAttack(player, attack_origin, player->physics.shared.facing));
            break;
        case WeaponType::Hammer:
            state.objects.createObject(new BigAttack(player, attack_origin, player->physics.shared.facing));
            break;
        }
        this->attacked_time = std::chrono::system_clock::now();
        this->resetAttack = false;
    }

    // Item::useItem(other, state, itemSelected);
}

void Weapon::reset(ServerGameState& state) {
    auto now = std::chrono::system_clock::now();
    std::chrono::duration<double> elapsed_milliseconds{ now - this->attacked_time };
    if ((now - this->attacked_time) > std::chrono::milliseconds(this->delay)) {
        this->resetAttack = true;
    }
}