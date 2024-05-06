#include "shared/game/sharedobject.hpp"

std::string objectTypeString(ObjectType type) {
	switch (type) {
	case ObjectType::Object:
		return "Object";
	case ObjectType::Item:
		return "Item";
	case ObjectType::SolidSurface:
		return "SolidSurface";
	default:
		return "Unknown";
	}
}