#include "server/game/orb.hpp"
#include "server/game/constants.hpp"
#include "shared/game/sharedobject.hpp"
#include <chrono>
#include "server/game/exit.hpp"

Orb::Orb(glm::vec3 corner, glm::vec3 dimensions) : Item(ObjectType::Orb, true, corner, ModelType::Orb, dimensions) {}

void Orb::doCollision(Object* other, ServerGameState& state) {
	//	Player can pick up the orb
	Item::doCollision(other, state);
	
	//	If the other object is a Player and
	//	if the phase is MatchPhase::MazeExploration, update the match phase to
	//	MatchPhase::RelayRace
	if (other->type == ObjectType::Player
		&& state.getMatchPhase() == MatchPhase::MazeExploration) {
		Player* player = state.objects.getPlayer(other->typeID);

		player->sharedInventory.hasOrb = true;

		state.setMatchPhase(MatchPhase::RelayRace);

		//	If anything else needs to be set when phase changes, can do it here
		//	Open all exits!
		for (int i = 0; i < state.objects.getExits().size(); i++) {
			Exit* exit = state.objects.getExits().get(i);

			if (exit == nullptr)
				continue;

			exit->shared.open = true;
		}
	}
}

// TODO: MAY NOT BE NEEDED AT ALL
void Orb::useItem(Object* other, ServerGameState& state, int itemSelected) {
	auto player = dynamic_cast<Player*>(other);
	Item::dropItem(other, state, itemSelected, 0.0f);
	this->physics.velocity = 0.8f * glm::normalize(other->physics.shared.facing);
	state.objects.moveObject(this, this->physics.shared.corner + glm::vec3(0.0f, 3.0f, 0.0f));
}