#pragma once
#include "server/game/object.hpp"
#include "shared/game/sharedobject.hpp"

class SolidSurface : public Object {
public:
	SolidSurface(bool movable, Collider collider, SurfaceType type, glm::vec3 corner, glm::vec3 dimensions);

	~SolidSurface();

	SharedSolidSurface shared{};

	virtual SharedObject toShared() override;
};