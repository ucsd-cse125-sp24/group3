#pragma once

#include "server/game/spell.hpp"
#include "server/game/servergamestate.hpp"
#include "server/game/objectmanager.hpp"
#include "server/game/object.hpp"

Spell::Spell(glm::vec3 corner, glm::vec3 dimensions):
    Item(ObjectType::Spell, false, corner, ModelType::Cube, dimensions)
{
    this->spellType = SpellType::Fireball;
}

void Spell::setSpellType(SpellType type) {
    this->spellType = type;
}