#pragma once

/* Container for bone data */

#include <vector>
#include <assimp/scene.h>
#include <list>
#include <glm/glm.hpp>

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/quaternion.hpp>
#include "client/util.hpp"

struct keyPosition
{
	glm::vec3 position;
	float timeStamp;
};

struct keyRotation
{
	glm::quat orientation;
	float timeStamp;
};

struct keyScale
{
	glm::vec3 scale;
	float timeStamp;
};

class Bone
{
public:
	Bone(const std::string& name, int ID, const aiNodeAnim* channel);
	
	void update(float animationTime);

	glm::mat4 getLocalTransform() { return m_localTransform; }
	std::string getBoneName() const { return m_name; }
	int getBoneID() { return m_ID; }
	
	int getPositionIndex(float animationTime);

	int getRotationIndex(float animationTime);

	int getScaleIndex(float animationTime);

private:

	float getScaleFactor(float lastTimeStamp, float nextTimeStamp, float animationTime);

	glm::mat4 interpolatePosition(float animationTime);

	glm::mat4 interpolateRotation(float animationTime);

	glm::mat4 interpolateScaling(float animationTime);

	std::vector<keyPosition> m_positions;
	std::vector<keyRotation> m_rotations;
	std::vector<keyScale> m_scales;
	int m_numPositions;
	int m_numRotations;
	int m_numScalings;

	glm::mat4 m_localTransform;
	std::string m_name;
	int m_ID;
};
