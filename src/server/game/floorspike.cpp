#include "server/game/floorspike.hpp"
#include "server/game/object.hpp"
#include "shared/game/sharedmodel.hpp"

const int FloorSpike::DAMAGE = 1;

FloorSpike::FloorSpike(glm::vec3 corner, float grid_width):
    Trap(ObjectType::FloorSpike, false, corner, Collider::Box, ModelType::FloorSpikeFull, glm::vec3(0.0f))
{ }

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