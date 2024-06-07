#pragma once

#include "assimp/aabb.h"
#include "assimp/types.h"
#include <glm/glm.hpp>

#include "shared/game/sharedobject.hpp"

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtc/quaternion.hpp>

/**
 * Convert ASSIMP color to a glm::vec3
 *
 * @param ASSIMP color struct
 * @return glm::vec3 of (r, g, b)
 */
glm::vec3 aiColorToGLM(const aiColor3D& color);

glm::mat4 matrixToGLM(const aiMatrix4x4& from);

glm::vec3 getGLMVec(const aiVector3D& vec);
/**
 * Bbox struct which stores the bounding box 
 * of an object.
 *
 * Only need to store two corners of the bounding 
 * box.
 */
struct Bbox {
    glm::vec3 getDimensions();
    std::pair<glm::vec3, glm::vec3> corners;
};

Bbox aiBboxToGLM(const aiAABB& bbox);

/**
 * Combine two bounding boxes together to get the bounding
 * box that encompasses the space both of them take up.
 *
 * Note that 
 *
 * @param bbox1 is a bounding box
 * @param bbox2 is another bounding box
 * @return is the combined bounding box which contains both
 * of the provided bounding boxes
 */
Bbox combineBboxes(const Bbox& bbox1, const Bbox& bbox2);

glm::quat getGLMQuat(const aiQuaternion& pOrientation);

// thx chat gpt
glm::vec3 rotate90DegreesAroundYAxis(const glm::vec3& direction);
glm::vec3 rotate90DegreesAroundXAxis(const glm::vec3& direction);
