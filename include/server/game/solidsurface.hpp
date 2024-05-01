#pragma once
#include "server/game/object.hpp"
#include "shared/game/sharedobject.hpp"

class SolidSurface : public Object {
public:
	SolidSurface();

	~SolidSurface();

	SharedSolidSurface shared{};

	virtual SharedObject toShared() override;
};