#pragma once
#include "server/game/object.hpp"
#include "shared/game/sharedobject.hpp"

class SolidSurface : public Object {
public:
	/**
	 * @param movable Whether or not the surface is affected by velocity
	 * @param collider Collision type for this object
	 * @param type Type of surface for this object, possibly used for different rendering options
	 * @param corner Corner position of the surface
	 * @param dimensions dimensions of the surface
	 */
	SolidSurface(bool movable, Collider collider, SurfaceType type, glm::vec3 corner, glm::vec3 dimensions);
	~SolidSurface();

	void setSurfaceType(SurfaceType type);

	SharedSolidSurface shared{};

	virtual SharedObject toShared() override;

	void setDMHighlight(bool highlight);
};