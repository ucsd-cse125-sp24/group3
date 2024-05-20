#pragma once

#include "client/core.hpp"
#include "client/renderable.hpp"

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/transform.hpp>
#include <glm/gtx/string_cast.hpp>
#include <glm/gtx/euler_angles.hpp>

#include <iostream>
#include <ostream>
#include <vector>

class Cube : public Renderable {
public:
    explicit Cube(glm::vec3 newColor);
    ~Cube();

    void draw(std::shared_ptr<Shader> shader,
            glm::mat4 viewProj,
            glm::vec3 camPos, 
            std::set<SharedObject, CompareLightPos>& lightSources,
            bool fill) override;
private:
    GLuint VAO;
    GLuint VBO_positions, VBO_normals, EBO;

    glm::vec3 color;

    // Cube Information
    std::vector<glm::vec3> positions;
    std::vector<glm::vec3> normals;
    std::vector<unsigned int> indices;

};