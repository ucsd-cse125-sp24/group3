#include "server/game/item.hpp"
#include "shared/game/sharedobject.hpp"
#include "server/game/servergamestate.hpp"
#include "server/game/objectmanager.hpp"
#include "shared/audio/constants.hpp"

/*  Constructors and Destructors    */
Item::Item(ObjectType type, bool movable, glm::vec3 corner, ModelType model, glm::vec3 dimensions):
    Object(type, Physics(movable, Collider::Box, corner, dimensions), ModelType::Cube),
	iteminfo(SharedItemInfo{ .held = false, .used = false })
{}

void Item::useItem(Object* other, ServerGameState& state, int itemSelected) {

	auto player = dynamic_cast<Player*>(other);
	if (player == nullptr) return; // only allow players to use items

	if (this->iteminfo.used) {
		player->inventory[itemSelected] = -1;
		player->sharedInventory.inventory[itemSelected] = ModelType::Frame;
	}
}

void Item::dropItem(Object* other, ServerGameState& state, int itemSelected, float dropDistance) {

	auto player = dynamic_cast<Player*>(other);
	if (player == nullptr) return; // only allow players to drop items

	this->iteminfo.held = false;
	this->physics.collider = Collider::Box;
	state.objects.moveObject(this, (player->physics.shared.corner + (player->physics.shared.facing * dropDistance)) * glm::vec3(1.0f, 0.0f, 1.0f));

	player->inventory[itemSelected] = -1;
	player->sharedInventory.inventory[itemSelected] = ModelType::Frame;

	state.soundTable().addNewSoundSource(SoundSource(
		ServerSFX::ItemDrop,
		other->physics.shared.getCenterPosition(),
		DEFAULT_VOLUME,
		SHORT_DIST,
		SHORT_ATTEN
	));
}

void Item::doCollision(Object* other, ServerGameState& state) {

	auto player = dynamic_cast<Player*>(other);
	if (player == nullptr) return; // only allow players to pick up items

	bool pickedUp = false;
	for (int i = 0; i < player->inventory.size(); i++) {
		if (player->inventory[i] != -1) { continue; }

		player->inventory[i] = this->typeID;
		player->sharedInventory.inventory[i] = this->modelType;
		pickedUp = true;
		break;
	}

	if (pickedUp) {
		this->iteminfo.held = true;
		this->physics.collider = Collider::None;
		state.soundTable().addNewSoundSource(SoundSource(
			ServerSFX::ItemPickUp,
			other->physics.shared.getCenterPosition(),
			DEFAULT_VOLUME,
			SHORT_DIST,
			SHORT_ATTEN
		));

		// update cell type in game state to empty
		GridCell* cell = state.getGrid().getCell(this->physics.shared.corner.x / Grid::grid_cell_width, this->physics.shared.corner.z / Grid::grid_cell_width);

		cell->type = CellType::Empty;
	}
}

/*	SharedGameState generation	*/
SharedObject Item::toShared() {
    auto so = Object::toShared();
    so.iteminfo = this->iteminfo;
    return so;
}