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
    Teleport,
};

class Spell : public Item {
public:

    /**
     * @param corner     Corner position of the Potion
     * @param dimensions Dimensions applied for the Potion
     * @param type       Type of Spell
     */
    Spell(glm::vec3 corner, glm::vec3 dimensions, SpellType type);

    SpellType spellType;
    int castLimit; 

    void useItem(Object* other, ServerGameState& state, int itemSelected) override;
    void doCollision(Object* other, ServerGameState& state) override;

private:

};