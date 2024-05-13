#include "client/renderable.hpp"
#include "glm/fwd.hpp"

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

void Renderable::scaleRelative(const float& new_factor) {
    glm::vec3 scaleVector(new_factor, new_factor, new_factor);
    this->model = glm::scale(this->model, scaleVector);
}

void Renderable::scaleRelative(const glm::vec3& scale) {
    this->model = glm::scale(this->model, scale);
}

void Renderable::scaleAbsolute(const float& new_factor) {
    this->model[0][0] = new_factor;
    this->model[1][1] = new_factor;
    this->model[2][2] = new_factor;
}

void Renderable::scaleAbsolute(const glm::vec3& scale) {
    this->model[0][0] = scale.x;
    this->model[1][1] = scale.y;
    this->model[2][2] = scale.z;
}

glm::mat4 Renderable::getModelMat() {
    return this->model;
}

void Renderable::clear() {
    this->model = glm::mat4(1.0f);
}

void Renderable::clearScale() {
    this->scaleAbsolute(1.0f);
}

void Renderable::clearPosition() {
    this->translateAbsolute(glm::vec3(0.0f, 0.0f, 0.0f));
}
