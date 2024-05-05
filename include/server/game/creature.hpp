#pragma once

#include "server/game/constants.hpp"
#include "server/game/object.hpp"
#include "shared/game/sharedobject.hpp"

class Creature : public Object {
public:
	explicit Creature(ObjectType type);
	~Creature();

	virtual SharedObject toShared() override;
private:

};