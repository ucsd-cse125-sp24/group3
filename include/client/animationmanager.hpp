#pragma once

#include <glm/glm.hpp>
#include <map>
#include <vector>
#include <assimp/scene.h>
#include <assimp/Importer.hpp>
#include "client/animation.hpp"
#include "client/bone.hpp"

class AnimationManager
{
public:
	AnimationManager(Animation* animation);

	void updateAnimation(float dt);

	void playAnimation(Animation* pAnimation);

	void calculateBoneTransform(const AssimpNodeData* node, glm::mat4 parentTransform);

	std::vector<glm::mat4> getFinalBoneMatrices() { return m_finalBoneMatrices; }

private:
	std::vector<glm::mat4> m_finalBoneMatrices;
	Animation* m_currentAnimation;
	float m_currentTime;
	float m_deltaTime;

};