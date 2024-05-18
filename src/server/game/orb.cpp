#pragma once

#include "server/game/orb.hpp"
#include "server/game/constants.hpp"
#include "shared/game/sharedobject.hpp"
#include <chrono>


Orb::Orb(glm::vec3 corner, glm::vec3 dimensions):
    Item(ObjectType::Orb, false, corner, ModelType::Cube, dimensions)
{
    this->modelType = ModelType::Orb;
}

void Orb::useItem(Object* other, ServerGameState& state, int itemSelected) {
    auto player = dynamic_cast<Player*>(other);

    this->iteminfo.used = true;
    this->iteminfo.held = false;

    Item::useItem(other, state, itemSelected);
}