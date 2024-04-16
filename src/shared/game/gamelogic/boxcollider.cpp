#include "client/boxcollider.hpp"
#include "client/spherecollider.hpp"
#include <math.h> 
#include <algorithm>

BoxCollider::BoxCollider() : 
	min_position(glm::vec3(0.0f)), max_position(glm::vec3(0.0f)) {
}

BoxCollider::BoxCollider(glm::vec3 minPos, glm::vec3 maxPos) :
	min_position(minPos), max_position(maxPos) {
}


BoxCollider::~BoxCollider() {
}

bool BoxCollider::detectCollision(Collider* otherCollider) {
	switch (otherCollider->getShape()) {
        case Box: {
			BoxCollider* otherC = dynamic_cast<BoxCollider*>(otherCollider);
			return (this->max_position.x >= otherC->min_position.x &&
					this->min_position.x <= otherC->max_position.x &&
					this->max_position.y >= otherC->min_position.y &&
					this->min_position.y <= otherC->max_position.y &&
					this->max_position.z >= otherC->min_position.z &&
					this->min_position.z <= otherC->max_position.z);
		}
		case Sphere: {
			SphereCollider* otherC = dynamic_cast<SphereCollider*>(otherCollider);
			float x = fmaxf(this->min_position.x, fminf(otherC->center.x, this->max_position.x));
			float y = fmaxf(this->min_position.y, fminf(otherC->center.y, this->max_position.y));
			float z = fmaxf(this->min_position.z, fminf(otherC->center.z, this->max_position.z));
			
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
