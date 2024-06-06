#include "server/game/lava.hpp"
#include "server/game/object.hpp"
#include "shared/game/sharedmodel.hpp"

const int Lava::DAMAGE = 1;

Lava::Lava(glm::vec3 corner, ModelType model_type, float grid_width):
    Trap(ObjectType::Lava, false, corner, Collider::Box, model_type, glm::vec3(0.0f))
{
    if (model_type == ModelType::LavaHorizontal) {
        this->physics.shared.dimensions.z /= 2.0f;
    } else if (model_type == ModelType::LavaVertical ) {
        this->physics.shared.dimensions.x /= 2.0f;
    }
}

bool Lava::shouldTrigger(ServerGameState& state) {
    return false;
}

bool Lava::shouldReset(ServerGameState& state) {
    return false;
}

void Lava::doCollision(Object* obj, ServerGameState& state) {
    if (this->info.dm_hover) {
        return;
    }

    auto creature = dynamic_cast<Creature*>(obj);
    if (creature == nullptr) return;

    creature->stats.health.decrease(DAMAGE);
}
