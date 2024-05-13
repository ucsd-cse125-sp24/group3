#pragma once

#include "server/game/constants.hpp"
#include "server/game/object.hpp"
#include "shared/game/sharedobject.hpp"

class Torchlight : public Object {
public:

	Torchlight();
	~Torchlight();

	virtual SharedObject toShared() override;
private:

};
