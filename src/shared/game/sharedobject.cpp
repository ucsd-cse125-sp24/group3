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