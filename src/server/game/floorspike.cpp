#include "server/game/floorspike.hpp"
#include "server/game/object.hpp"
#include "shared/game/sharedmodel.hpp"

const int FloorSpike::DAMAGE = 1;

FloorSpike::FloorSpike(glm::vec3 corner, FloorSpike::Orientation orientation, float grid_width):
    Trap(ObjectType::FloorSpike, false, corner, Collider::Box, ModelType::Cube, glm::vec3(0.0f))
{
    const float HEIGHT = 1.0f;

    this->physics.shared.dimensions.y = HEIGHT;
    this->physics.shared.dimensions.x = grid_width;
    this->physics.shared.dimensions.z = grid_width;

    if (orientation == FloorSpike::Orientation::Horizontal) {
        this->physics.shared.dimensions.z /= 2.0f;
        // this->modelType = ModelType::FloorSpikeHorizontal;
        // this->setModel(this->modelType);
    } else if (orientation == FloorSpike::Orientation::Vertical) {
        this->physics.shared.dimensions.x /= 2.0f;
        // this->modelType = ModelType::FloorSpikeVertical;
        // this->setModel(this->modelType);
    }
}

bool FloorSpike::shouldTrigger(ServerGameState& state) {
    return false;
}

bool FloorSpike::shouldReset(ServerGameState& state) {
    return false;
}

void FloorSpike::doCollision(Object* obj, ServerGameState& state) {
    if (this->info.dm_hover) {
        return;
    }

    auto creature = dynamic_cast<Creature*>(obj);
    if (creature == nullptr) return;

    creature->stats.health.decrease(DAMAGE);
}