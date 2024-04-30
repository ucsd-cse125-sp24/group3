#pragma once

#include "client/core.hpp"

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/transform.hpp>
#include <glm/gtx/string_cast.hpp>
#include <glm/gtx/euler_angles.hpp>

#include <iostream>
#include <ostream>
#include <vector>

class Cube {
public:
    Cube(glm::vec3 newColor, glm::vec3 scale = glm::vec3(1.0f));
    ~Cube();
    void draw(GLuint shader, bool fill);
    void update(glm::vec3 new_pos);
    void update_delta(glm::vec3 delta);

private:
    GLuint VAO;
    GLuint VBO_positions, VBO_normals, EBO;

    glm::mat4 model;    
    glm::vec3 color;


    // Cube Information
    std::vector<glm::vec3> positions;
    std::vector<glm::vec3> normals;
    std::vector<unsigned int> indices;

};