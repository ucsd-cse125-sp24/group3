#include "server/game/item.hpp"
#include "shared/game/sharedobject.hpp"
#include "server/game/servergamestate.hpp"
#include "server/game/objectmanager.hpp"

/*  Constructors and Destructors    */
Item::Item(ObjectType type, bool movable, glm::vec3 corner, ModelType model, glm::vec3 dimensions):
    Object(type, Physics(movable, Collider::Box, corner, dimensions), ModelType::Cube),
	iteminfo(SharedItemInfo{ .held = false, .used = false })
{}

void Item::useItem(Object* other, ServerGameState& state) {
}

void Item::dropItem(Object* other, ServerGameState& state, float dropDistance) {

	auto player = dynamic_cast<Player*>(other);
	if (player == nullptr) return; // only allow players to drop items

	this->iteminfo.held = false;
	this->physics.collider = Collider::Box;
	this->physics.shared.corner = (player->physics.shared.corner + (player->physics.shared.facing * dropDistance)) * glm::vec3(1.0f, 0.0f, 1.0f);
}

void Item::doCollision(Object* other, ServerGameState& state) {

	auto player = dynamic_cast<Player*>(other);
	if (player == nullptr) return; // only allow players to pick up items

	if (player->inventory.size() < player->sharedInventory.inventory_size) {
		for (int i = 1; i <= player->sharedInventory.inventory_size; i++) {
			if (!player->inventory.contains(i)) {
				player->inventory[i] = this->typeID;
				player->sharedInventory.inventory[i] = this->modelType;
				break;
			}
		}
		this->iteminfo.held = true;		
		this->physics.collider = Collider::None;
	}
}

/*	SharedGameState generation	*/
SharedObject Item::toShared() {
    auto so = Object::toShared();
    so.iteminfo = this->iteminfo;
    return so;
}