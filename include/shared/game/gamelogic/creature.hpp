#pragma once

#include "constants.hpp"
#include "object.hpp"

class Creature : public Object {
public:
	float health;

	float speed;

	Creature();
	~Creature();

	void walk();

	void attack();

private:

};