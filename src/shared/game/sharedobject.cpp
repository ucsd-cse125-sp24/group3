#include "shared/game/sharedobject.hpp"
#include "shared/game/status.hpp"


std::string objectTypeString(ObjectType type) {
	switch (type) {
	case ObjectType::Object:
		return "Object";
    case ObjectType::SolidSurface:
        return "SolidSurface";
    case ObjectType::Player:
        return "Player";
    case ObjectType::Slime:
        return "Slime";
    case ObjectType::Potion:
        return "Potion";
    case ObjectType::Enemy:
        return "Enemy";
    case ObjectType::Torchlight:
        return "Torchlight";
    case ObjectType::SpikeTrap:
        return "SpikeTrap";
    case ObjectType::DungeonMaster:
        return "DungeonMaster";
    case ObjectType::FireballTrap:
        return "FireballTrap";
    case ObjectType::Projectile:
        return "Projectile";
    case ObjectType::FloorSpike:
        return "FloorSpike";
    case ObjectType::FakeWall:
        return "FakeWall";
    case ObjectType::ArrowTrap:
        return "ArrowTrap";
    case ObjectType::TeleporterTrap:
        return "TeleporterTrap";
    case ObjectType::Spell:
        return "Spell";
    case ObjectType::Minotaur:
        return "Minotaur";
    case ObjectType::Python:
        return "Python";
    case ObjectType::Item:
        return "Item";
    case ObjectType::Exit:
        return "Exit";
    case ObjectType::Orb:
        return "Orb";
    case ObjectType::Weapon:
        return "Weapon";
    case ObjectType::WeaponCollider:
        return "WeaponCollider";
	default:
		return "Unknown";
	}
}

glm::vec3 SharedPhysics::getCenterPosition() const {
    return this->corner + (this->dimensions / 2.0f);
}

void SharedStatuses::addStatus(Status status, size_t len) {
    if (this->map.contains(status)) {
        this->map.at(status) += len;
    } else {
        this->map.insert({status, len});
    }
}

void SharedStatuses::tickStatus() {
    for (auto& [status, len] : this->map) {
        if (len > 0) {
            len--;
        }
    }
}

std::vector<std::string> SharedStatuses::getStatusStrings() const {
    std::vector<std::string> statuses;
    for (const auto& [status, len] : this->map) {
        if (len > 0) {
            statuses.push_back(getStatusString(status, len));
        }
    }
    if (statuses.empty()) {
        statuses.push_back("Unafflicted");
    }
    return statuses;
}

size_t SharedStatuses::getStatusLength(Status status) const {
    if (this->map.contains(status)) {
        return this->map.at(status);
    } else {
        return 0;
    }
}
