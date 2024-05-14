#pragma once

#include "server/game/servergamestate.hpp"
#include "server/game/object.hpp"
#include "server/game/item.hpp"

/*
 *  Different types of spells
 */
enum class SpellType {
	Fireball,
    HealOrb,
};

class Spell : public Item {
public:

    /**
     * @param corner    Corner position of the Spell
     * @param model     Model applied for the Spell
     * @param type      Type of Spell
     */
    Spell(glm::vec3 corner, glm::vec3 dimensions, SpellType type);

    SpellType spellType;
    int castLimit; 

    void useItem(Object* other, ServerGameState& state) override;

private:

};