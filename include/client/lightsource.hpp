#pragma once

#include "client/core.hpp"
#include "client/shader.hpp"

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/transform.hpp>
#include <glm/gtx/string_cast.hpp>
#include <glm/gtx/euler_angles.hpp>

#include <iostream>
#include <ostream>
#include <vector>
#include <memory>

class LightSource {
public:
    LightSource();
    void draw(std::shared_ptr<Shader> shader, glm::mat4 viewProj);
    void TranslateTo(const glm::vec3& new_pos);

    glm::vec3 lightPos;
private:
    GLuint VAO, VBO;

    glm::mat4 model;
    glm::vec3 color;

    // Cube Information
    std::vector<glm::vec3> positions;
    std::vector<glm::vec3> normals;
    std::vector<unsigned int> indices;

};
