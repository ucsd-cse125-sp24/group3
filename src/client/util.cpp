#include "client/util.hpp"

glm::vec3 aiColorToGLM(const aiColor3D& color) {
    return glm::vec3(color.r, color.g, color.b);
}

