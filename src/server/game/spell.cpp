#pragma once

#include "server/game/spell.hpp"
#include "server/game/servergamestate.hpp"
#include "server/game/objectmanager.hpp"
#include "server/game/object.hpp"

Spell::Spell():
    Item(ObjectType::Spell)

    this->spellType = SpellType::Fireball;
    this->iteminfo.type = ItemType::Spell;
}

void Spell::setSpellType(SpellType type) {
    this->spellType = type;
}