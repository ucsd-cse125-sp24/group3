#include "client/animation.hpp"

Animation::Animation(const std::string& animationPath, Model* model) {
    Assimp::Importer importer;
    const aiScene* scene = importer.ReadFile(animationPath, 
        aiProcess_Triangulate);
    assert(scene && scene->mRootNode);
    auto animation = scene->mAnimations[0];
    m_duration = animation->mDuration;
    m_ticksPerSecond = animation->mTicksPerSecond;
    aiMatrix4x4 globalTransformation = scene->mRootNode->mTransformation;
    globalTransformation = globalTransformation.Inverse();
    readHierarchyData(m_rootNode, scene->mRootNode);
    readMissingBones(animation, *model);
}

Animation::Animation(const std::string& animationDirPath, const std::string& animName, int frames):
    m_rootNode({})
{
    // not used by this constructor
    m_duration = 0;
    m_ticksPerSecond = 0;

    for (int i = 1; i <= frames; i++) {
        auto frame_model_path = animationDirPath + "/" + animName + std::to_string(i) + ".obj";
        auto frame_model = new Model(frame_model_path, true);
        model_keyframes.push_back(frame_model);
    }
}

Bone* Animation::findBone(const std::string& name) {
    auto iter = std::find_if(m_bones.begin(), m_bones.end(),
        [&](const Bone& Bone)
        {
            return Bone.getBoneName() == name;
        }
    );
    if (iter == m_bones.end()) return nullptr;
    else return &(*iter);
}


void Animation::readMissingBones(const aiAnimation* animation, Model& model) {
    int size = animation->mNumChannels;

    auto& boneInfoMap = model.getBoneInfoMap(); //getting m_BoneInfoMap from Model class
    int& boneCount = model.getBoneCount(); //getting the m_BoneCounter from Model class

    //reading channels(bones engaged in an animation and their keyframes)
    for (int i = 0; i < size; i++)
    {
        auto channel = animation->mChannels[i];
        std::string boneName = channel->mNodeName.data;

        if (boneInfoMap.find(boneName) == boneInfoMap.end())
        {
            boneInfoMap[boneName].id = boneCount;
            boneCount++;
        }
        m_bones.push_back(Bone(channel->mNodeName.data,
            boneInfoMap[channel->mNodeName.data].id, channel));
    }

    m_boneInfoMap = boneInfoMap;
}

void Animation::readHierarchyData(AssimpNodeData& dest, const aiNode* src) {
    assert(src);

    dest.name = src->mName.data;
    dest.transformation = matrixToGLM(src->mTransformation);
    dest.numChildren = src->mNumChildren;

    for (int i = 0; i < src->mNumChildren; i++)
    {
        AssimpNodeData newData;
        readHierarchyData(newData, src->mChildren[i]);
        dest.children.push_back(newData);
    }
}

