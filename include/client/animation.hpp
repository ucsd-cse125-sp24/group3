#pragma once

#include <vector>
#include <map>
#include <glm/glm.hpp>
#include <assimp/scene.h>
#include <functional>
#include "client/bone.hpp"
#include "client/model.hpp"
#include "client/util.hpp"

struct AssimpNodeData {
	glm::mat4 transformation;
	std::string name;
	int numChildren;
	std::vector<AssimpNodeData> children;
};

class Animation {
public:
	Animation() = default;

	Animation(const std::string& animationPath, Model* model);

	~Animation() {}

	Bone* findBone(const std::string& name);
	
	inline float getTicksPerSecond() { return m_ticksPerSecond; }
	inline float getDuration() { return m_duration;}
	inline const AssimpNodeData& getRootNode() { return m_rootNode; }
	inline const std::map<std::string,BoneInfo>& getBoneIDMap() { return m_boneInfoMap; }

private:
	void readMissingBones(const aiAnimation* animation, Model& model);

	void readHierarchyData(AssimpNodeData& dest, const aiNode* src);

	float m_duration;
	int m_ticksPerSecond;
	std::vector<Bone> m_bones;
	AssimpNodeData m_rootNode;
	std::map<std::string, BoneInfo> m_boneInfoMap;
};
