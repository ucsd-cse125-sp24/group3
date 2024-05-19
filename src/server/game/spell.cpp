#pragma once

#include "server/game/spell.hpp"
#include "server/game/item.hpp"
#include "server/game/constants.hpp"
#include "server/game/servergamestate.hpp"
#include "server/game/object.hpp"
#include "server/game/projectile.hpp"

Spell::Spell(glm::vec3 corner, glm::vec3 dimensions, SpellType spelltype):
    Item(ObjectType::Spell, false, corner, ModelType::Cube, dimensions)
{
    this->castLimit = CAST_LIMIT;
    this->spellType = spelltype;

    switch (spelltype) {
    case SpellType::Fireball:
        this->modelType = ModelType::FireSpell;
        break;
    case SpellType::HealOrb:
        this->modelType = ModelType::HealSpell;
        break;
    }
}

void Spell::useItem(Object* other, ServerGameState& state, int itemSelected) {
    Player* player = dynamic_cast<Player*>(other);

    glm::vec3 spell_origin(
        player->physics.shared.getCenterPosition().x,
        1.5f,
        player->physics.shared.getCenterPosition().z
    );

    spell_origin += player->physics.shared.facing * 2.0f;

    switch (this->spellType) {
    case SpellType::Fireball: {
        state.objects.createObject(new SpellOrb(spell_origin, player->physics.shared.facing, SpellType::Fireball));
        break;
    }
    case SpellType::HealOrb: {
        state.objects.createObject(new SpellOrb(spell_origin, player->physics.shared.facing, SpellType::HealOrb));
        break;
    }
    }

    this->castLimit -= 1;
    player->sharedInventory.usesRemaining[itemSelected] = this->castLimit;

    if (castLimit == 0) {
        this->iteminfo.used = true;
        this->iteminfo.held = false;

        state.markForDeletion(this->globalID);
    }

    Item::useItem(other, state, itemSelected);
}

void Spell::doCollision(Object* other, ServerGameState& state) {

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