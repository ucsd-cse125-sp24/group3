#include  "shared/game/GameLogic/CollisionBoundary.hpp"

CollisionBoundary::CollisionBoundary() {
	CollisionBoundary(glm::vec3(0.0f), glm::vec3(0.0f));
}

CollisionBoundary::CollisionBoundary(glm::vec3 minPos, glm::vec3 maxPos) {
	this->min_position = minPos;
	this->max_position = masPos;

	glm::vec3 v3 = minPos + maxPos;
	this->center = v3 / 2;
}

CollisionBoundary::~CollisionBoundary() {
}

void CollisionBoundary::detectCollision(CollisionBoundary otherBoundary) {
	return(	this->max_position.x > otherBoundary.min_position.x &&
			this->min_position.x < otherBoundary.max_position.x &&
			this->max_position.y > otherBoundary.min_position.y &&
			this->min_position.y < otherBoundary.max_position.y &&
			this->max_position.z > otherBoundary.min_position.z &&
			this->min_position.z < otherBoundary.max_position.z);
}

void CollisionBoundary::resolveCollision(CollisionBoundary otherBoundary) {

}