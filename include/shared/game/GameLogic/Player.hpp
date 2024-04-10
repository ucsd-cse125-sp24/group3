#pragma once

#include "constants.hpp"
#include "Item.hpp"
#include "Creature.hpp"

class Player : public Creature {
public:
	array<Item, MAX_ITEMS> items;

	Player();
	~Player();

	void useItem();

	void pickItem();

private:

};