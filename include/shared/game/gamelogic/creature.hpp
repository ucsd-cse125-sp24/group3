#pragma once

#include "constants.hpp"
#include "object.hpp"

struct Stats {
	float health;
	float speed;
};

class Creature : public Object {
public:
	Stats stats;

	Creature();
	~Creature();

	virtual SharedObject generateSharedObject() override;

private:

};