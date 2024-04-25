#include "shared/game/sharedobject.hpp"

std::string objectTypeString(ObjectType type) {
	switch (type) {
	case ObjectType::Object:
		return "Object";
	default:
		return "Unknown";
	}
}