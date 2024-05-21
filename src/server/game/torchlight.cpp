
#include "server/game/torchlight.hpp"
#include "glm/fwd.hpp"
#include "server/game/collider.hpp"
#include "server/game/object.hpp"
#include "shared/game/sharedobject.hpp"
#include "shared/utilities/rng.hpp"

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
        .flickering = true,
        .min_intensity = 0.3f,
        .max_intensity = 1.0f,
        .ambient_color = glm::vec3(0.5f, 0.25f, 0.015f),
        .diffuse_color = glm::vec3(1.0f, 0.5f, 0.03f),
        .specular_color = glm::vec3(0.5f, 0.25f, 0.015f),
        .attenuation_linear = 0.07f,
        .attenuation_quadratic = 0.017f
    };
    init();
}

Torchlight::Torchlight(
    glm::vec3 corner,
    const TorchlightProperties& properties):
	Object(ObjectType::Torchlight, Physics(false, 
		Collider::Box, corner, glm::vec3(0.0f), glm::vec3(1.0f)),
		ModelType::Torchlight),
    properties(properties)
{
    init();
}

void Torchlight::init() {
    this->inc_intensity = true;
    // if not flickering, set intensity to a static 
    // value
    if (!this->properties.flickering) {
        this->curr_intensity = this->properties.max_intensity;
    } else {
        // if flickering randomize initial  
        // animation step to offset flickering
        this->curr_step = randomDouble(0.0f, 1.0f);
        this->flickering_speed = randomDouble(0.005f, 0.01f);
    }
}

Torchlight::~Torchlight() {}

bool Torchlight::doTick(ServerGameState& state) {
    if(!this->properties.flickering) {
        return false;
    }

    // either increment or decrement intensity
    if (inc_intensity) {
        this->curr_step += this->flickering_speed;
    } else {
        this->curr_step -= this->flickering_speed;
    }

    // toggle inc_intensity for next tick
    if (this->curr_step >= 1.0f) {
        this->inc_intensity = false;
    }
    if (this->curr_step <= 0.0f) {
        this->inc_intensity = true;
    }

    // non-linear function for setting intensity over
    // time. taken from https://easings.net/#easeOutBack
    const float c1 = 1.70158;
    const float c3 = c1 + 1;
    this->curr_intensity = 1 + c3 * std::pow(this->curr_step - 1, 3) + c1 * std::pow(curr_step - 1, 2);

    // cut off intensities outside desired range
    if (this->curr_intensity > this->properties.max_intensity) {
        this->curr_intensity = this->properties.max_intensity;
    } else if (this->curr_intensity < this->properties.min_intensity) {
        this->curr_intensity = this->properties.min_intensity;
    }
    return true;
}
