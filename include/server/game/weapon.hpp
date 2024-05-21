#pragma once

#include "server/game/servergamestate.hpp"
#include "server/game/object.hpp"
#include "server/game/item.hpp"
#include "server/game/collider.hpp"

/*
 *  Different types of spells
 */
enum class WeaponType {
	Dagger,
    Katana,
    Hammer,
};

class Weapon : public Item {
public:

    /**
     * @param corner     Corner position of the weapon
     * @param dimensions Dimensions applied for the weapon
     * @param type       Type of weapon
     */
    Spell(glm::vec3 corner, glm::vec3 dimensions, WeaponType type);

    WeaponType weaponType;

    void useItem(Object* other, ServerGameState& state, int itemSelected) override;

private:

};