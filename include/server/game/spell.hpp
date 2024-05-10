#pragma once

#include "server/game/item.hpp"
#include "server/game/servergamestate.hpp"
#include "server/game/object.hpp"

enum class SpellType {
	Fireball,
};

class Spell : public Item {
public:
    Potion();

    SpellType spellType;

    void setSpellType(SpellType type);

private:

};