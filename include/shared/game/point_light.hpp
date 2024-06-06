#pragma once

#include <glm/glm.hpp>

struct PointLightProperties {
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
