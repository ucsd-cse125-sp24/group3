#include "client/boxcollider.hpp"
#include "client/spherecollider.hpp"
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
			float x = fmaxf(otherC->min_position.x, fminf(this->center.x, otherC->max_position.x));
			float y = fmaxf(otherC->min_position.y, fminf(this->center.y, otherC->max_position.y));
			float z = fmaxf(otherC->min_position.z, fminf(this->center.z, otherC->max_position.z));
			
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
