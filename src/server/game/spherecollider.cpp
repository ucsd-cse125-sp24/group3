#include "server/game/boxcollider.hpp"
#include "server/game/spherecollider.hpp"
#include <math.h> 
#include <algorithm>

SphereCollider::SphereCollider() { 
	this->corner = corner;
	this->dimension = dimension;
}

SphereCollider::SphereCollider(glm::vec3 corner, glm::vec3 dimension) {
	this->corner = corner;
	this->dimension = dimension;
}

SphereCollider::~SphereCollider() {
}

bool SphereCollider::detectCollision(Collider* otherCollider) {
	glm::vec3 center = this->corner + (this->dimension / 2.0f);
	float radius = this->dimension.x / 2.0f;

	switch (otherCollider->getShape()) {
        case Sphere: {
			const SphereCollider* otherC = dynamic_cast<SphereCollider*>(otherCollider);
			glm::vec3 otherCenter = otherC->corner + (otherC->dimension / 2.0f);
			float otherRadius = otherC->dimension.x / 2.0f;

			float distance = sqrt(
				(center.x - otherCenter.x) * (center.x - otherCenter.x) +
				(center.y - otherCenter.y) * (center.y - otherCenter.y) +
				(center.z - otherCenter.z) * (center.z - otherCenter.z)
			);

			return distance < radius + otherRadius;
		}
		case Box: {
			const BoxCollider* otherC = dynamic_cast<BoxCollider*>(otherCollider);
			glm::vec3 otherMinPos = otherC->corner;
			glm::vec3 otherMaxPos = otherC->corner + otherC->dimension;

			float x = fmaxf(otherMinPos.x, fminf(center.x, otherMaxPos.x));
			float y = fmaxf(otherMinPos.y, fminf(center.y, otherMaxPos.y));
			float z = fmaxf(otherMinPos.z, fminf(center.z, otherMaxPos.z));
			
			float distance = sqrt(
				(x - center.x) * (x - center.x) +
				(y - center.y) * (y - center.y) +
				(z - center.z) * (z - center.z)
			);

			return distance < radius;
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
