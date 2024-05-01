#include "server/game/solidsurface.hpp"

/*	Constructors and Destructors	*/
SolidSurface::SolidSurface() : Object(ObjectType::SolidSurface) {

}

SolidSurface::~SolidSurface() {}

/*	SharedGameState generation	*/
SharedObject SolidSurface::toShared() {
	SharedObject shared = Object::toShared();

	shared.solidSurface = this->shared;

	return shared;
}