#include "server/game/lava.hpp"
#include "server/game/object.hpp"
#include "shared/game/point_light.hpp"
#include "shared/game/sharedmodel.hpp"

const int Lava::DAMAGE = 1;

Lava::Lava(glm::vec3 corner, ModelType model_type, float grid_width, const PointLightProperties& light_properties):
    Trap(ObjectType::Lava, false, corner, Collider::Box, model_type, glm::vec3(0.0f)),
    light_properties(light_properties)
{
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

SharedObject Lava::toShared() {
	auto so = Object::toShared();
    so.pointLightInfo = SharedPointLightInfo {
        .intensity = 1.0f,
        .ambient_color = this->light_properties.ambient_color,
        .diffuse_color = this->light_properties.diffuse_color,
        .specular_color = this->light_properties.specular_color,
        .attenuation_linear = this->light_properties.attenuation_linear,
        .attenuation_quadratic = this->light_properties.attenuation_quadratic
    };
	return so;
}
