#include "server/game/exit.hpp"
#include "server/game/servergamestate.hpp"
#include "shared/game/point_light.hpp"
#include "shared/game/sharedgamestate.hpp"

Exit::Exit(bool open, glm::vec3 corner, glm::vec3 dimensions, const PointLightProperties& properties) 
	: Object(ObjectType::Exit,
	Physics(false, Collider::Box, corner, glm::vec3(0.0f), dimensions), ModelType::Cube), 
    properties(properties)
{
	this->shared.open = open;
	this->intensity = 1.0f;
}

void Exit::setIntensity(float val) {
	this->intensity = val;
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
    so.pointLightInfo = SharedPointLightInfo {
        .intensity = this->intensity,
        .ambient_color = this->properties.ambient_color,
        .diffuse_color = this->properties.diffuse_color,
        .specular_color = this->properties.specular_color,
        .attenuation_linear = this->properties.attenuation_linear,
        .attenuation_quadratic = this->properties.attenuation_quadratic
    };
	return so;
}