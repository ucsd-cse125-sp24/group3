#include "server/game/orb.hpp"
#include "server/game/exit.hpp"

Orb::Orb(glm::vec3 corner, glm::vec3 dimensions) : Item(ObjectType::Orb, true, corner, ModelType::Cube, dimensions) {}

void Orb::doCollision(Object* other, ServerGameState& state) {
	//	Play can pick up the orb
	Item::doCollision(other, state);

	//	If the phase is MatchPhase::MazeExploration, update the match phase to
	//	MatchPhase::RelayRace
	if (state.getMatchPhase() == MatchPhase::MazeExploration) {
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