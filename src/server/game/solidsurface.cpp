#include "server/game/solidsurface.hpp"

/*	Constructors and Destructors	*/
SolidSurface::SolidSurface() : Object(ObjectType::SolidSurface) {
	//	Set collider to Box
	this->physics.collider = Collider::Box;
}

SolidSurface::~SolidSurface() {}

void SolidSurface::setSurfaceType(SurfaceType type) {
	this->shared.surfaceType = type;
}

/*	SharedGameState generation	*/
SharedObject SolidSurface::toShared() {
	SharedObject sharedSolidSurface = Object::toShared();

	sharedSolidSurface.solidSurface = this->shared;

	return sharedSolidSurface;
}