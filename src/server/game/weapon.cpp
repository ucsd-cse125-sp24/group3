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

    switch (weaponType) {
    case WeaponType::Katana:
        this->modelType = ModelType::Katana;
        break;
    case WeaponType::Dagger:
        this->modelType = ModelType::Dagger;
        break;
    case WeaponType::Hammer:
        this->modelType = ModelType::Hammer;
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

    switch (weaponType) {
    case WeaponType::Katana:
        state.objects.createObject(new LongAttacks(player, attack_origin, player->physics.shared.facing));
        break;
    case WeaponType::Dagger:
        state.objects.createObject(new ShortAttacks(player, attack_origin, player->physics.shared.facing));
        break;
    case WeaponType::Hammer:
        state.objects.createObject(new BigAttacks(player, attack_origin, player->physics.shared.facing));
        break;
    }

    // Item::useItem(other, state, itemSelected);
}