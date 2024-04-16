#pragma once

#include "constants.hpp"
#include "item.hpp"
#include "creature.hpp"

class Player : public Creature {
public:
	array<Item, MAX_ITEMS> items;

	Player();
	~Player();

	void useItem();

	void pickItem();

private:

};