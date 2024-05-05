#include "server/game/boxcollider.hpp"
#include "server/game/spherecollider.hpp"
#include <math.h> 
#include <algorithm>

BoxCollider::BoxCollider() {
	this->corner = corner;
	this->dimension = dimension;
}

BoxCollider::BoxCollider(glm::vec3 corner, glm::vec3 dimension) {
	this->corner = corner;
	this->dimension = dimension;
}


BoxCollider::~BoxCollider() {
}

bool BoxCollider::detectCollision(Collider* otherCollider) {
	glm::vec3 minPos = this->corner;
	glm::vec3 maxPos = this->corner + this->dimension;

	switch (otherCollider->getShape()) {
        case Box: {
			BoxCollider* otherC = dynamic_cast<BoxCollider*>(otherCollider);
			glm::vec3 otherMinPos = otherC->corner;
			glm::vec3 otherMaxPos = otherC->corner + otherC->dimension;


			return (maxPos.x >= otherMinPos.x &&
					minPos.x <= otherMaxPos.x &&
					maxPos.y >= otherMinPos.y &&
					minPos.y <= otherMaxPos.y &&
					maxPos.z >= otherMinPos.z &&
					minPos.z <= otherMaxPos.z);
		}
		case Sphere: {
			const SphereCollider* otherC = dynamic_cast<SphereCollider*>(otherCollider);
			glm::vec3 otherCenter = otherC->corner + (otherC->dimension / 2.0f);
			float otherRadius = otherC->dimension.x / 2.0f;

			float x = fmaxf(minPos.x, fminf(otherCenter.x, maxPos.x));
			float y = fmaxf(minPos.y, fminf(otherCenter.y, maxPos.y));
			float z = fmaxf(minPos.z, fminf(otherCenter.z, maxPos.z));
			
			float distance = sqrt(
				(x - otherCenter.x) * (x - otherCenter.x) +
				(y - otherCenter.y) * (y - otherCenter.y) +
				(z - otherCenter.z) * (z - otherCenter.z)
			);

			return distance < otherRadius;
		}
	}
	return false;
}

bool BoxCollider::resolveCollision(Collider* otherCollider) {
	return true;
}

Collider::Shape BoxCollider::getShape(){
	return Box;
};
