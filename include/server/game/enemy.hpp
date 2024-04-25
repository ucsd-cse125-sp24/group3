#pragma once

#include "server/game/constants.hpp"
#include "server/game/object.hpp"
#include "server/game/creature.hpp"
#include "shared/game/sharedobject.hpp"



class Enemy : public Creature {
public:
	Enemy();
	~Enemy();

	virtual SharedObject toShared() override;
private:

};