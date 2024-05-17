#include "shared/game/sharedobject.hpp"


std::string objectTypeString(ObjectType type) {
	switch (type) {
	case ObjectType::Object:
		return "Object";
    case ObjectType::SolidSurface:
        return "SolidSurface";
    case ObjectType::Player:
        return "Player";
    case ObjectType::Enemy:
        return "Enemy";
	default:
		return "Unknown";
	}
}

glm::vec3 SharedPhysics::getCenterPosition() {
    return this->corner + (this->dimensions / 2.0f);
}

void sortObjectsByPos(std::vector<SharedObject>& objects, glm::vec3 refPos) {
    struct ComparePos {
        explicit ComparePos(glm::vec3& refPos) : refPos(refPos) {}
        bool operator()(SharedObject& a, SharedObject& b) const {
            // manhattan distance because it's cheaper to compute than euclidian.
            // also, only need to compare distance in x and z dimensions since
            // everything in the maze is about the same height
            float distanceToA = std::abs(this->refPos.x - a.physics.corner.x) +
                std::abs(this->refPos.z - a.physics.corner.z);
            float distanceToB = std::abs(this->refPos.x - b.physics.corner.x) +
                std::abs(this->refPos.z - b.physics.corner.z);

            return distanceToA < distanceToB;
        }
        glm::vec3 refPos;
    };

    std::sort(objects.begin(), objects.end(), ComparePos(refPos));
}