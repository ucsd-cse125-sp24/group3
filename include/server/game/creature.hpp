#pragma once

#include "server/game/constants.hpp"
#include "server/game/object.hpp"
#include "shared/game/sharedobject.hpp"



class Creature : public Object {
public:
	Stats stats;

	Creature();
	~Creature();

	virtual SharedObject generateSharedObject() override;
private:

};