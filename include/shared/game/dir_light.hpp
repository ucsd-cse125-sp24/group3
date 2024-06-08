#pragma once

#include <glm/glm.hpp>

/**
 * @brief DirLight holds lighting information
 * for directional light source. Note that this
 * is different than point lights which emit from
 * a certain point.
 */
struct DirLight {
    glm::vec3 direction;

    glm::vec3 ambient_color;
    glm::vec3 diffuse_color;
    glm::vec3 specular_color;
};
