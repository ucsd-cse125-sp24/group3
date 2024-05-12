#pragma once

#include "server/game/servergamestate.hpp"
#include "server/game/object.hpp"

enum class SpellType {
	Fireball,
};

class Spell : public Item {
public:
    Spell(glm::vec3 corner, glm::vec3 dimensions);

    SpellType spellType;

    void setSpellType(SpellType type);

private:

};