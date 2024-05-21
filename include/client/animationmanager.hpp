#pragma once

#include <glm/glm.hpp>
#include <map>
#include <vector>
#include <unordered_map>
#include <assimp/scene.h>
#include <assimp/Importer.hpp>
#include "client/animation.hpp"
#include "client/bone.hpp"
#include "shared/game/sharedobject.hpp"

class AnimationManager
{
public:
	AnimationManager(Animation* animation);

	void updateAnimation(float dt);

	void playAnimation(Animation* pAnimation);

	void calculateBoneTransform(const AssimpNodeData* node, glm::mat4 parentTransform);

	void addAnimation(Animation* anim, ObjectType objType, AnimState animState);

	void setAnimation(ObjectType objType, AnimState animState);

	std::vector<glm::mat4> getFinalBoneMatrices() { return m_finalBoneMatrices; }

private:
	std::vector<glm::mat4> m_finalBoneMatrices;
	std::unordered_map<ObjectType, std::unordered_map<AnimState, Animation*>> objAnimMap;
	Animation* m_currentAnimation;
	float m_currentTime;
	float m_deltaTime;

};