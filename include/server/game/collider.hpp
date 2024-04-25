#pragma once

#include "server/game/constants.hpp"

class Collider {
public:

	enum Shape {   
		Box, 
		Sphere
	};

	virtual bool detectCollision(Collider* otherCollider) = 0;
	virtual bool resolveCollision(Collider* otherCollider) = 0;
	virtual Shape getShape() = 0;

private:

};