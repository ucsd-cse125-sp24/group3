#pragma once

#include <glm/glm.hpp>
#include <map>
#include <vector>
#include <unordered_map>
#include <utility>
#include <random>

#include <assimp/scene.h>
#include <assimp/Importer.hpp>
#include "client/animation.hpp"
#include "client/bone.hpp"
#include "shared/game/sharedobject.hpp"

class AnimationManager
{
public:
	explicit AnimationManager(Animation* animation);

	void updateAnimation(float dt);

	Model* updateFrameAnimation(float dt);

	void playAnimation(Animation* pAnimation);

	void calculateBoneTransform(const AssimpNodeData* node, glm::mat4 parentTransform);

	void addAnimation(Animation* anim, ObjectType objType, AnimState animState);

	void setAnimation(EntityID id, ObjectType objType, AnimState animState);

	void setFrameAnimation(EntityID id, ObjectType objType, AnimState animState);

	std::vector<glm::mat4> getFinalBoneMatrices() { return m_finalBoneMatrices; }

private:
	std::vector<glm::mat4> m_finalBoneMatrices;
	std::unordered_map<EntityID, std::pair<int, Animation*>> entityAnimFrameMap;
	std::unordered_map<EntityID, std::pair<float, Animation*>> entityAnimMap;
	std::unordered_map<ObjectType, std::unordered_map<AnimState, Animation*>> objAnimMap;
	Animation* m_currentAnimation;
	EntityID currEntity;
	float m_currentTime;
	float m_deltaTime;
	int currFrame;
};