#include "server/game/collider.hpp"
#include "server/game/object.hpp"

bool detectCollision(Physics& obj1, Physics& obj2) {
	switch (obj1.collider) {
		case Collider::Sphere:
			return detectCollisionSphere(obj1, obj2);
		case Collider::Box:
			return detectCollisionBox(obj1, obj2);

		//	If the object doesn't have a collider, the collision detection
		//	always returns false
		case Collider::None:
		default:
			return false;
	}
}

bool detectCollisionSphere(Physics& sphere, Physics& obj) {
	glm::vec3 center = sphere.shared.corner + (sphere.shared.dimensions / 2.0f);
	float radius = sphere.shared.dimensions.x / 2.0f;

	switch (obj.collider) {
		case Collider::Sphere: {
			glm::vec3 objCenter = 
				obj.shared.corner + (obj.shared.dimensions / 2.0f);
			float objRadius = obj.shared.dimensions.x / 2.0f;

			float distance = sqrt(
				(center.x - objCenter.x) * (center.x - objCenter.x) +
				(center.y - objCenter.y) * (center.y - objCenter.y) +
				(center.z - objCenter.z) * (center.z - objCenter.z)
			);

			return distance < radius + objRadius;
		}
		case Collider::Box: {
			glm::vec3 objMinPos = obj.shared.corner;
			glm::vec3 objMaxPos = obj.shared.corner + obj.shared.dimensions;

			float x = fmaxf(objMinPos.x, fminf(center.x, objMaxPos.x));
			float y = fmaxf(objMinPos.y, fminf(center.y, objMaxPos.y));
			float z = fmaxf(objMinPos.z, fminf(center.z, objMaxPos.z));

			float distance = sqrt(
				(x - center.x) * (x - center.x) +
				(y - center.y) * (y - center.y) +
				(z - center.z) * (z - center.z)
			);

			return distance < radius;
		}
		//	If the object doesn't have a collider, the collision detection
		//	always returns false
		case Collider::None:
		default:
			return false;
	}

	return false;
}

bool detectCollisionBox(Physics& box, Physics& obj) {
	glm::vec3 minPos = box.shared.corner;
	glm::vec3 maxPos = box.shared.corner + box.shared.dimensions;

	switch (obj.collider) {
		case Collider::Sphere: {
			glm::vec3 objCenter =
				obj.shared.corner + (obj.shared.dimensions / 2.0f);
			float objRadius = obj.shared.dimensions.x / 2.0f;

			float x = fmaxf(minPos.x, fminf(objCenter.x, maxPos.x));
			float y = fmaxf(minPos.y, fminf(objCenter.y, maxPos.y));
			float z = fmaxf(minPos.z, fminf(objCenter.z, maxPos.z));

			float distance = sqrt(
				(x - objCenter.x) * (x - objCenter.x) +
				(y - objCenter.y) * (y - objCenter.y) +
				(z - objCenter.z) * (z - objCenter.z)
			);

			return distance < objRadius;
		}
		case Collider::Box: {
			glm::vec3 objMinPos = obj.shared.corner;
			glm::vec3 objMaxPos = obj.shared.corner + obj.shared.dimensions;

			return (maxPos.x >= objMinPos.x &&
				minPos.x <= objMaxPos.x &&
				maxPos.y >= objMinPos.y &&
				minPos.y <= objMaxPos.y &&
				maxPos.z >= objMinPos.z &&
				minPos.z <= objMaxPos.z);
		}
		//	If the object doesn't have a collider, the collision detection
		//	always returns false
		case Collider::None:
		default:
			return false;
	}
}