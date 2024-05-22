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
    Sword,
    Hammer,
};

class Weapon : public Item {
public:

    /**
     * @param corner     Corner position of the weapon
     * @param dimensions Dimensions applied for the weapon
     * @param type       Type of weapon
     */
    Weapon(glm::vec3 corner, glm::vec3 dimensions, WeaponType type);

    WeaponType weaponType;

    void useItem(Object* other, ServerGameState& state, int itemSelected) override;
    void reset(ServerGameState& state);

private:
    int delay;
    bool resetAttack;
    std::chrono::time_point<std::chrono::system_clock> attacked_time;
};