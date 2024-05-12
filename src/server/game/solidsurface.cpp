#include "server/game/solidsurface.hpp"

/*	Constructors and Destructors	*/
SolidSurface::SolidSurface() : Object(ObjectType::SolidSurface) {
	//	Set collider to Box
	this->physics.collider = Collider::Box;
}

SolidSurface::~SolidSurface() {}

/*	SharedGameState generation	*/
SharedObject SolidSurface::toShared() {
	SharedObject sharedSolidSurface = Object::toShared();

	sharedSolidSurface.solidSurface = this->shared;

	return sharedSolidSurface;
}