#pragma once

#include "assimp/types.h"
#include <glm/glm.hpp>

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtc/quaternion.hpp>

glm::vec3 aiColorToGLM(const aiColor3D& color);

glm::mat4 matrixToGLM(const aiMatrix4x4& from);

glm::vec3 getGLMVec(const aiVector3D& vec);

glm::quat getGLMQuat(const aiQuaternion& pOrientation);