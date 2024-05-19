#include "server/game/exit.hpp"
#include "server/game/servergamestate.hpp"
#include "shared/game/sharedgamestate.hpp"

Exit::Exit(bool open, glm::vec3 corner, glm::vec3 dimensions) 
	: Object(ObjectType::Exit,
	Physics(false, Collider::Box, corner, glm::vec3(0.0f), dimensions), ModelType::Cube)
{
	this->shared.open = open;
}

void Exit::doCollision(Object* other, ServerGameState& state) {
	//	If the object colliding with the exit is a:
	//		1. Player
	//		2. This exit is open
	//		3. The match phase is MatchPhase::RelayRace
	//		4. The Player is holding the Orb
	//	Then, the game is won for the players!
	if (other->type == ObjectType::Player
		&& this->shared.open
		&& state.getMatchPhase() == MatchPhase::RelayRace) {
		//	Get Player object
		Player* player = state.objects.getPlayer(other->typeID);

		if (player->sharedInventory.hasOrb) {
			state.setPlayerVictory(true);
			state.setPhase(GamePhase::RESULTS);
		}
	}
}

/*	SharedGameState generation	*/
SharedObject Exit::toShared() {
	auto so = Object::toShared();
	so.exit = this->shared;
	return so;
}