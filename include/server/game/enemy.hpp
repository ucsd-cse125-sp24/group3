#pragma once

#include "server/game/constants.hpp"
#include "server/game/object.hpp"
#include "server/game/creature.hpp"
#include "shared/game/sharedobject.hpp"

struct EnemyStats {
	int health;


};

class Enemy : public Creature {
public:
	EnemyStats stats;

	// list of abilities

	Enemy();
	~Enemy();

	virtual SharedObject toShared() override;
private:

};