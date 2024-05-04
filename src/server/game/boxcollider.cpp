#include "server/game/boxcollider.hpp"
#include "server/game/spherecollider.hpp"
#include <math.h> 
#include <algorithm>

BoxCollider::BoxCollider() : 
	corner(glm::vec3(0.0f)), dimension(glm::vec3(0.0f)) {
}

BoxCollider::BoxCollider(glm::vec3 corner, glm::vec3 dimension) :
	corner(corner), dimension(dimension) {
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
			float x = fmaxf(minPos.x, fminf(otherC->center.x, maxPos.x));
			float y = fmaxf(minPos.y, fminf(otherC->center.y, maxPos.y));
			float z = fmaxf(minPos.z, fminf(otherC->center.z, maxPos.z));
			
			float distance = sqrt(
				(x - otherC->center.x) * (x - otherC->center.x) +
				(y - otherC->center.y) * (y - otherC->center.y) +
				(z - otherC->center.z) * (z - otherC->center.z)
			);

			return distance < otherC->radius;
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
