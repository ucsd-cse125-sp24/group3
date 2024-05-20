#pragma once

#include <glm/glm.hpp>

#include "shared/game/sharedobject.hpp"

// struct CompareLightPos {
//     CompareLightPos() = default;
//     CompareLightPos(const glm::vec3& refPos) : refPos(refPos) {};
//     bool operator()(const SharedObject& a, const SharedObject& b) const {
//         float distanceToA = glm::distance(this->refPos, a.physics.corner);
//         float distanceToB = glm::distance(this->refPos, b.physics.corner);
//         return distanceToA < distanceToB;
//     };
//     glm::vec3 refPos;
// };
