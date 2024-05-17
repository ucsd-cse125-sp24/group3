
#include "server/game/torchlight.hpp"
#include "glm/fwd.hpp"
#include "server/game/collider.hpp"
#include "server/game/object.hpp"
#include "shared/game/sharedobject.hpp"

SharedObject Torchlight::toShared() {
    auto so = Object::toShared();
    so.pointLightInfo = SharedPointLightInfo {
        .intensity = this->curr_intensity,
        .ambient_color = this->properties.ambient_color,
        .diffuse_color = this->properties.diffuse_color,
        .specular_color = this->properties.specular_color,
        .attenuation_linear = this->properties.attenuation_linear,
        .attenuation_quadratic = this->properties.attenuation_quadratic,
    };
    return so;
}

Torchlight::Torchlight(
    glm::vec3 corner):
	Object(ObjectType::Torchlight, Physics(false, 
		Collider::Box, corner, glm::vec3(0.0f), glm::vec3(1.0f)),
		ModelType::Torchlight)
{
    this->properties = TorchlightProperties {
        .flickering = false,
        .min_intensity = 0.3f,
        .max_intensity = 1.0f,
        .flickering_speed = 0.05f,
        .ambient_color = glm::vec3(0.5f, 0.25f, 0.015f),
        .diffuse_color = glm::vec3(1.0f, 0.5f, 0.03f),
        .specular_color = glm::vec3(0.5f, 0.25f, 0.015f),
        .attenuation_linear = 0.07f,
        .attenuation_quadratic = 0.017f
    };
    if (!this->properties.flickering) {
        this->curr_intensity = this->properties.max_intensity;
    }
}

Torchlight::Torchlight(
    glm::vec3 corner,
    TorchlightProperties properties):
	Object(ObjectType::Torchlight, Physics(false, 
		Collider::Box, corner, glm::vec3(0.0f), glm::vec3(1.0f)),
		ModelType::Torchlight),
    properties(properties)
{
    if (!this->properties.flickering) {
        this->curr_intensity = this->properties.max_intensity;
    }
}

Torchlight::~Torchlight() {}

bool Torchlight::doTick(ServerGameState& state) {
    if(!this->properties.flickering) {
        return false;
    }

    // either increment or decrement intensity
    if (inc_intensity) {
        this->curr_time += this->properties.flickering_speed;
    } else {
        this->curr_time -= this->properties.flickering_speed;
    }

    // toggle inc_intensity for next tick
    if (this->curr_intensity >= 1.0f) {
        this->inc_intensity = false;
    }
    if (this->curr_intensity <= 0.0f) {
        this->inc_intensity = true;
    }

    // non-linear function for setting intensity over
    // time
    curr_intensity = pow(2, 10 * this->curr_time - 10);
    return true;
}
