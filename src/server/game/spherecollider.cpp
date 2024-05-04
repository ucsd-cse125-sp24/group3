#include "server/game/boxcollider.hpp"
#include "server/game/spherecollider.hpp"
#include <math.h> 
#include <algorithm>

SphereCollider::SphereCollider() : 
	center(glm::vec3(0.0f)), radius(0) {
}

SphereCollider::SphereCollider(glm::vec3 center, float radius) : 
	center(center), radius(radius) {
}

SphereCollider::~SphereCollider() {
}

bool SphereCollider::detectCollision(Collider* otherCollider) {
	switch (otherCollider->getShape()) {
        case Sphere: {
			const SphereCollider* otherC = dynamic_cast<SphereCollider*>(otherCollider);
			float distance = sqrt(
				(this->center.x - otherC->center.x) * (this->center.x - otherC->center.x) +
				(this->center.y - otherC->center.y) * (this->center.y - otherC->center.y) +
				(this->center.z - otherC->center.z) * (this->center.z - otherC->center.z)
			);

			return distance < this->radius + otherC->radius;
		}
		case Box: {
			const BoxCollider* otherC = dynamic_cast<BoxCollider*>(otherCollider);
			glm::vec3 otherMinPos = otherC->corner;
			glm::vec3 otherMaxPos = otherC->corner + otherC->dimension;

			float x = fmaxf(otherMinPos.x, fminf(this->center.x, otherMaxPos.x));
			float y = fmaxf(otherMinPos.y, fminf(this->center.y, otherMaxPos.y));
			float z = fmaxf(otherMinPos.z, fminf(this->center.z, otherMaxPos.z));
			
			float distance = sqrt(
				(x - this->center.x) * (x - this->center.x) +
				(y - this->center.y) * (y - this->center.y) +
				(z - this->center.z) * (z - this->center.z)
			);

			return distance < this->radius;
		}
	}
	return false;
}

bool SphereCollider::resolveCollision(Collider* otherCollider) {
	return true;
}

Collider::Shape SphereCollider::getShape(){
	return Sphere;
};
