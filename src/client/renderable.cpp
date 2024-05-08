#include "client/renderable.hpp"

#include <glm/glm.hpp>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/transform.hpp>

Renderable::Renderable() : model(1.0f) { }

void Renderable::translateAbsolute(const glm::vec3 &new_pos) {
    this->model[3] = glm::vec4(new_pos, 1.0f);
}

void Renderable::translateRelative(const glm::vec3& delta) {
    this->model = glm::translate(this->model, delta);
}

void Renderable::scale(const float& new_factor) {
    glm::vec3 scaleVector(new_factor, new_factor, new_factor);
    this->model = glm::scale(this->model, scaleVector);
}

void Renderable::scale(const glm::vec3& scale) {
    this->model = glm::scale(this->model, scale);
}

glm::mat4 Renderable::getModelMat() {
    return this->model;
}
