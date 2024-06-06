#include "server/game/orb.hpp"
#include "server/game/constants.hpp"
#include "shared/game/sharedobject.hpp"
#include "server/game/exit.hpp"
#include "shared/audio/constants.hpp"
#include <chrono>

Orb::Orb(glm::vec3 corner, glm::vec3 dimensions) : Item(ObjectType::Orb, true, corner, ModelType::Cube, dimensions) {
	this->modelType = ModelType::Orb;
}

void Orb::doCollision(Object* other, ServerGameState& state) {

	if (other->type == ObjectType::Exit) {
		state.setPlayerVictory(true);
		state.setPhase(GamePhase::RESULTS);
	}

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
		player->sharedInventory.hasOrb = true;
		this->iteminfo.held = true;
		this->physics.collider = Collider::None;
		state.transitionToRelayRace();

		state.soundTable().addNewSoundSource(SoundSource(
			ServerSFX::ItemPickUp,
			other->physics.shared.getCenterPosition(),
			DEFAULT_VOLUME,
			SHORT_DIST,
			SHORT_ATTEN
		));
	}
}

void Orb::useItem(Object* other, ServerGameState& state, int itemSelected) {
	auto player = dynamic_cast<Player*>(other);
	player->sharedInventory.hasOrb = false;
	Item::dropItem(other, state, itemSelected, 3.0f);
	this->physics.velocity = 0.8f * glm::normalize(other->physics.shared.facing);
	state.objects.moveObject(this, this->physics.shared.corner + glm::vec3(0.0f, 3.0f, 0.0f));

	// check to make sure that not colliding with anything
	// lazy copy paste with below... keep in sync
	auto grid_cells = state.objects.objectGridCells(this);
	for (glm::ivec2 grid_cell : grid_cells) {
		auto potential_collision_objects = state.objects.cellToObjects.at(grid_cell);	
		for (Object* obj : potential_collision_objects) {
			if (obj->type != ObjectType::Orb && obj->type != ObjectType::Player && detectCollision(this->physics, obj->physics)) {
				// go back in the inventory b/c inside a wall or something
				this->doCollision(player, state); // get picked up by the player again
				return;
			}
		}
	}
}

void Orb::dropItem(Object* other, ServerGameState& state, int itemSelected, float dropDistance) {
	auto player = dynamic_cast<Player*>(other);

	Item::dropItem(other, state, itemSelected, dropDistance);
	player->sharedInventory.hasOrb = false;

	if (dropDistance == 0.0f) {
		return;
	}

	// make sure isn't in wall
	// lazy copy paste with above... keep in sync
	auto grid_cells = state.objects.objectGridCells(this);
	for (glm::ivec2 grid_cell : grid_cells) {
		auto potential_collision_objects = state.objects.cellToObjects.at(grid_cell);	
		for (Object* obj : potential_collision_objects) {
			if (obj->type != ObjectType::Orb && obj->type != ObjectType::Player && detectCollision(this->physics, obj->physics)) {
				// go back in the inventory b/c inside a wall or something
				this->doCollision(player, state); // get picked up by the player again
				return;
			}
		}
	}
}