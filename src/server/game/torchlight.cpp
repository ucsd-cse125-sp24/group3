
#include "server/game/torchlight.hpp"
#include "glm/fwd.hpp"
#include "server/game/collider.hpp"
#include "server/game/object.hpp"
#include "shared/game/point_light.hpp"
#include "shared/game/sharedobject.hpp"
#include "shared/utilities/rng.hpp"
#include "server/game/grid.hpp"

SharedObject Torchlight::toShared() {
    auto so = Object::toShared();
    so.pointLightInfo = SharedPointLightInfo {
        .intensity = this->curr_intensity,
        .ambient_color = this->properties.ambient_color,
        .diffuse_color = this->properties.diffuse_color,
        .specular_color = this->properties.specular_color,
        .attenuation_linear = this->properties.attenuation_linear,
        .attenuation_quadratic = this->properties.attenuation_quadratic
    };
    return so;
}

Torchlight::Torchlight(
    glm::vec3 corner, float dist_orb, float dist_exit):
	Object(ObjectType::Torchlight, Physics(false, 
		Collider::Box, corner, glm::vec3(0.0f), glm::vec3(1.0f)),
		ModelType::Torchlight)
{
    const float MIN_ORB_DIST = Grid::grid_cell_width * 60.0f;
    const float MIN_EXIT_DIST = Grid::grid_cell_width * 60.0f; // min distance to start shade white

    const float AMBER_MIN_INTENSITY = 0.3f;
    const float AMBER_MAX_INTENSITY = 1.0f;
    const glm::vec3 AMBER_AMBIENT(0.05f, 0.05f, 0.05f);
    const glm::vec3 AMBER_DIFFUSE(1.0f, 0.5f, 0.03f);
    const glm::vec3 AMBER_SPECULAR(0.5f, 0.25f, 0.015f);

    const float BLUE_MIN_INTENSITY = 0.1f;
    const float BLUE_MAX_INTENSITY = 0.5f;
    const glm::vec3 BLUE_AMBIENT(0.0f, 0.75f, 0.67f);
    const glm::vec3 BLUE_DIFFUSE(0.0f, 0.75f, 0.67f);
    const glm::vec3 BLUE_SPECULAR(0.0f, 0.35, 0.33f);

    const float WHITE_MIN_INTENSITY = 0.1f;
    const float WHITE_MAX_INTENSITY = 0.3f;
    const glm::vec3 WHITE_AMBIENT(1.05f, 1.05f, 1.05f);
    const glm::vec3 WHITE_DIFFUSE(1.0f, 1.0f, 1.0f);
    const glm::vec3 WHITE_SPECULAR(0.5f, 0.5f, 0.5f);

    const float ATTEN_LINEAR = 0.07f;
    const float ATTEN_QUAD = 0.017f;

    // higher intensity takes you closer to color2
    const auto interpolateColor = [](float intensity, glm::vec3 color1, glm::vec3 color2) {
        return color1 + intensity * (color2 - color1);
    };

    if (dist_orb < MIN_ORB_DIST) {
        // the closer you get, the higher the factor to make it more white is multiplied
        const float blue_intensity = (MIN_ORB_DIST - dist_orb) / MIN_ORB_DIST;

        // close to orb, so shade blue
        this->properties = PointLightProperties {
            .flickering = true,
            .min_intensity = BLUE_MIN_INTENSITY,
            .max_intensity = BLUE_MAX_INTENSITY,
            .ambient_color = interpolateColor(blue_intensity, AMBER_AMBIENT, BLUE_DIFFUSE),
            .diffuse_color = interpolateColor(blue_intensity, AMBER_DIFFUSE, BLUE_DIFFUSE),
            .specular_color = interpolateColor(blue_intensity, AMBER_SPECULAR, BLUE_SPECULAR),
            .attenuation_linear = ATTEN_LINEAR,
            .attenuation_quadratic = ATTEN_QUAD
        };
    } else if (dist_exit < MIN_EXIT_DIST) {
        const float white_intensity = (MIN_EXIT_DIST - dist_exit) / MIN_EXIT_DIST;
        // close to exit, so shade white
        // TEMP: still amber
        this->properties = PointLightProperties {
            .flickering = true,
            .min_intensity = WHITE_MIN_INTENSITY,
            .max_intensity = WHITE_MAX_INTENSITY,
            .ambient_color = interpolateColor(white_intensity, AMBER_AMBIENT, WHITE_AMBIENT),
            .diffuse_color = interpolateColor(white_intensity, AMBER_DIFFUSE, WHITE_DIFFUSE),
            .specular_color = interpolateColor(white_intensity, AMBER_SPECULAR, WHITE_SPECULAR),
            .attenuation_linear = ATTEN_LINEAR,
            .attenuation_quadratic = ATTEN_QUAD
        };
    } else {
        // shade normal amber
        this->properties = PointLightProperties {
            .flickering = true,
            .min_intensity = AMBER_MIN_INTENSITY,
            .max_intensity = AMBER_MAX_INTENSITY,
            .ambient_color = AMBER_AMBIENT,
            .diffuse_color = AMBER_DIFFUSE,
            .specular_color = AMBER_SPECULAR,
            .attenuation_linear = ATTEN_LINEAR,
            .attenuation_quadratic = ATTEN_QUAD
        };
    }
    
    init();
}

Torchlight::Torchlight(
    glm::vec3 corner,
    const PointLightProperties& properties):
	Object(ObjectType::Torchlight, Physics(false, 
		Collider::Box, corner, glm::vec3(0.0f), glm::vec3(1.0f)),
		ModelType::Torchlight),
    properties(properties)
{
    init();
}

void Torchlight::init() {
    this->is_cut = false;

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

void Torchlight::doTick(ServerGameState& state, std::optional<glm::vec3> lightning_light_cut_pos, std::optional<glm::vec3> action_light_cut_pos) {
    if(!this->properties.flickering) {
        return;
    }

    // cut this light if within position of light cut
    if (lightning_light_cut_pos.has_value()) {
        glm::vec3 pos = lightning_light_cut_pos.value();

        // if within threshold, get out
        if (glm::distance(pos, this->physics.shared.getCenterPosition()) <= LIGHT_CUT_RANGE) {
            this->curr_intensity = 0.0f;
            return;
        }
    }

    if (action_light_cut_pos.has_value()) {
        glm::vec3 pos = action_light_cut_pos.value();

        // if within threshold, black out (slightly expand the light cut action range)
        if (glm::distance(pos, this->physics.shared.getCenterPosition()) <= (1.5 * LIGHT_CUT_RANGE)) {
            this->curr_intensity = 0.0f;
            this->is_cut = true;
            return;
        }
    }

    this->is_cut = false;

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


void Torchlight::overrideIntensity(float val) {
    this->curr_intensity = val;
}