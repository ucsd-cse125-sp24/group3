#include "server/game/orb.hpp"
#include "server/game/constants.hpp"
#include "shared/game/sharedobject.hpp"
#include <chrono>
#include "server/game/exit.hpp"

Orb::Orb(glm::vec3 corner, glm::vec3 dimensions) : Item(ObjectType::Orb, true, corner, ModelType::Cube, dimensions) {
	this->modelType = ModelType::Orb;
}

void Orb::doCollision(Object* other, ServerGameState& state) {
	//	Player can pick up the orb
	Item::doCollision(other, state);
	
	//	If the other object is a Player, then the Player picks up the Orb
	if (other->type == ObjectType::Player) {
		Player* player = state.objects.getPlayer(other->typeID);

		player->sharedInventory.hasOrb = true;

		//	update match phase to MatchPhase::RelayRace
		state.transitionToRelayRace();
	} else if (other->type == ObjectType::Exit) {
		state.setPlayerVictory(true);
		state.setPhase(GamePhase::RESULTS);
	}
}

// TODO: MAY NOT BE NEEDED AT ALL
void Orb::useItem(Object* other, ServerGameState& state, int itemSelected) {
	auto player = dynamic_cast<Player*>(other);
	dropItem(other, state, itemSelected, 0.0f);
	this->physics.velocity = 0.8f * glm::normalize(other->physics.shared.facing);
	state.objects.moveObject(this, this->physics.shared.corner + glm::vec3(0.0f, 3.0f, 0.0f));
}

void Orb::dropItem(Object* other, ServerGameState& state, int itemSelected, float dropDistance) {
	Item::dropItem(other, state, itemSelected, 3.0f);

	//	Player dropped the orb
	Player* player = state.objects.getPlayer(other->typeID);

	player->sharedInventory.hasOrb = false;

	std::cout << "Player has dropped the Orb!" << std::endl;
}