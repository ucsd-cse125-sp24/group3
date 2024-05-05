#include "server/game/solidsurface.hpp"

/*	Constructors and Destructors	*/
SolidSurface::SolidSurface() : Object(ObjectType::SolidSurface) {

}

SolidSurface::~SolidSurface() {}

/*	SharedGameState generation	*/
SharedObject SolidSurface::toShared() {
	SharedObject sharedSolidSurface = Object::toShared();

	sharedSolidSurface.solidSurface = this->shared;

	return sharedSolidSurface;
}