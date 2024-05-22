#include "client/renderable.hpp"
#include "glm/fwd.hpp"

#include <glm/glm.hpp>

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/transform.hpp>
#include <glm/gtx/string_cast.hpp>


Renderable::Renderable() : model(1.0f), rotation(1.0f, 0.0f, 0.0f, 0.0f) { }

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

void Renderable::rotateAbsolute(const glm::vec3& dir, const glm::vec3& axis) {
    float r = glm::atan(dir.x, dir.z);
    this->rotation = glm::angleAxis(r, axis);
}

void Renderable::rotateRelative(const glm::vec3& dir, const glm::vec3& axis) {
    // float rot = glm::acos(glm::dot(dir, this->facing));
    float r1 = glm::atan(dir.x, dir.z);
    float r2 = glm::angle(rotation);
    this->rotation = glm::rotate(rotation, r1 - r2, axis);
}

glm::mat4 Renderable::getModelMat() {
    return this->model * glm::mat4_cast(rotation);
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
