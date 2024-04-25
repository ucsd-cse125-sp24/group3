#pragma once

#include "server/game/constants.hpp"
#include "server/game/object.hpp"
#include "shared/game/sharedobject.hpp"

class Creature : public Object {
public:
	Creature();
	~Creature();

	virtual SharedObject toShared() override;
private:

};