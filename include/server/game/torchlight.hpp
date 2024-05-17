#pragma once

#include "server/game/constants.hpp"
#include "server/game/object.hpp"
#include "shared/game/sharedobject.hpp"

struct TorchlightProperties {
    // if the light should flicker or not. if set
    // to false, it will be constantly at the  
    // max_intensity
    bool flickering;
    // lowest intensity the light should reach during flickering.
    // range is from 0-1
    float min_intensity;
    // highest intensity the light should reach during flickering.
    // range is from 0-1. if flickering is false, this intensity 
    // will be used.
    float max_intensity;
    // how much the intensity should change on every server tick
    float flickering_speed; 

    // lighting properties of that the light source
    // emits
    glm::vec3 ambient_color;
    glm::vec3 diffuse_color;
    glm::vec3 specular_color; // shiny effects

    // these two affect the dropoff of the light intensity
    // as distance increases
    float attenuation_linear;
    float attenuation_quadratic;
};


class Torchlight : public Object {
public:
	/**
     * Creates a torchight with default lighting properties.
	 * @param corner Corner position of the surface
	 */
	Torchlight(glm::vec3 corner);

	/**
	 * @param corner Corner position of the surface
     * @param properties allows for customization of lighting
     * and flickering properties
	 */
	Torchlight(glm::vec3 corner, TorchlightProperties properties);
	~Torchlight();

    SharedObject toShared();

    bool doTick(ServerGameState& state) override;
private:
    TorchlightProperties properties;

    // current intensity from 0-1 that gets
    // sent to client
    float curr_intensity;

    // state needed for animated flickering 

    // curr_time from 0-1 in flickering animation
    float curr_time;
    // if the flickering animation is inceasing in
    // intensity or decreasing
    bool inc_intensity;
};
