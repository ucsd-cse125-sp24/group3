#include "server/game/solidsurface.hpp"

/*	Constructors and Destructors	*/
SolidSurface::SolidSurface(
	bool movable, Collider collider, SurfaceType type,
	glm::vec3 corner, glm::vec3 dimensions) :
	Object(ObjectType::SolidSurface, Physics(movable, 
		collider, corner, glm::vec3(0.0f), dimensions),
		ModelType::Cube)
{
	this->shared.surfaceType = type;
	this->shared.dm_highlight = false;
	this->shared.is_internal = false;
}

SolidSurface::~SolidSurface() {}

void SolidSurface::setSurfaceType(SurfaceType type) {
	this->shared.surfaceType = type;
}

void SolidSurface::setDMHighlight(bool highlight) {
	this->shared.dm_highlight = highlight;
}

/*	SharedGameState generation	*/
SharedObject SolidSurface::toShared() {
	SharedObject sharedSolidSurface = Object::toShared();

	sharedSolidSurface.solidSurface = this->shared;

	return sharedSolidSurface;
}