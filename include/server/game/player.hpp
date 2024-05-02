#pragma once

#include "server/game/constants.hpp"
#include "server/game/object.hpp"
#include "server/game/creature.hpp"
#include "shared/game/sharedobject.hpp"

struct PlayerStats {
	int health;


};


class Player : public Creature {
public:
	PlayerStats stats;

	// player can have items

	Player();
	~Player();

	virtual SharedObject toShared() override;
private:

};