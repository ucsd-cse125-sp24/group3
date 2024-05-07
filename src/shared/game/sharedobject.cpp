#include "shared/game/sharedobject.hpp"

std::string objectTypeString(ObjectType type) {
	switch (type) {
	case ObjectType::Object:
		return "Object";
    case ObjectType::Item:
        return "Item";
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