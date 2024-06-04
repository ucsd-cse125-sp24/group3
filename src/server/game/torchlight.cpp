
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
    // for amber orange lights
    this->properties = TorchlightProperties {
        .flickering = true,
        .min_intensity = 0.3f,
        .max_intensity = 1.0f,
        .ambient_color = glm::vec3(0.05f, 0.05f, 0.05f),
        .diffuse_color = glm::vec3(1.0f, 0.5f, 0.03f),
        .specular_color = glm::vec3(0.5f, 0.25f, 0.015f),
        .attenuation_linear = 0.07f,
        .attenuation_quadratic = 0.017f
    };
    
    // for blue lights
    // this->properties = TorchlightProperties {
    //     .flickering = true,
    //     .min_intensity = 0.1f,
    //     .max_intensity = 0.5f,
    //     .ambient_color = glm::vec3(0.0f, 0.75f, 0.67f),
    //     .diffuse_color = glm::vec3(0.0f, 0.75f, 0.67f),
    //     .specular_color = glm::vec3(0.0f, 0.35f, 0.33f),
    //     .attenuation_linear = 0.07f,
    //     .attenuation_quadratic = 0.017f
    // };
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
        this->flickering_speed = randomDouble(0.008, 0.014f);
    }
}

Torchlight::~Torchlight() {}

void Torchlight::doTick(ServerGameState& state, std::optional<glm::vec3> light_cut_pos) {
    if(!this->properties.flickering) {
        return;
    }

    // cut this light if within position of light cut
    if (light_cut_pos.has_value()) {
        glm::vec3 pos = light_cut_pos.value();

        // if within threshold, get out
        if (glm::distance(pos, this->physics.shared.getCenterPosition()) <= 100.0f) {
            this->curr_intensity = 0.0f;
            std::cout << "A LIGHT CUT OUT!" << std::endl;
            return;
        }
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
}

float Torchlight::getIntensity() const {
    return this->curr_intensity;
}
