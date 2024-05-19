#pragma once

#include "server/game/orb.hpp"
#include "server/game/constants.hpp"
#include "shared/game/sharedobject.hpp"
#include <chrono>


Orb::Orb(glm::vec3 corner, glm::vec3 dimensions):
    Item(ObjectType::Orb, false, corner, ModelType::Cube, dimensions)
{
    this->modelType = ModelType::Orb;
    this->physics.movable = true;
}

// TODO: MAY NOT BE NEEDED AT ALL
void Orb::useItem(Object* other, ServerGameState& state, int itemSelected) {
    auto player = dynamic_cast<Player*>(other);

    Item::dropItem(other, state, itemSelected, 0.0f);

    this->physics.velocity = 0.8f * glm::normalize(other->physics.shared.facing);
    state.objects.moveObject(this, this->physics.shared.corner + glm::vec3(0.0f, 3.0f, 0.0f));
}