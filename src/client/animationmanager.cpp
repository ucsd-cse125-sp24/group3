#include "client/animationmanager.hpp"

AnimationManager::AnimationManager(Animation* animation) {
    m_currentTime = 0.0;
    m_currentAnimation = animation;

    m_finalBoneMatrices.reserve(100);

    for (int i = 0; i < 100; i++)
        m_finalBoneMatrices.push_back(glm::mat4(1.0f));
}

void AnimationManager::updateAnimation(float dt) {
    m_deltaTime = dt;
    if (m_currentAnimation) {
        m_currentTime += m_currentAnimation->getTicksPerSecond() * dt;
        m_currentTime = fmod(m_currentTime, m_currentAnimation->getDuration());
        calculateBoneTransform(&m_currentAnimation->getRootNode(), glm::mat4(1.0f));
    }
}

void AnimationManager::playAnimation(Animation* pAnimation) {
    m_currentAnimation = pAnimation;
    m_currentTime = 0.0f;
}

void AnimationManager::calculateBoneTransform(const AssimpNodeData* node, glm::mat4 parentTransform) {
    std::string nodeName = node->name;
    glm::mat4 nodeTransform = node->transformation;

    Bone* bone = m_currentAnimation->findBone(nodeName);

    if (bone)
    {
        bone->update(m_currentTime);
        nodeTransform = bone->getLocalTransform();
    }

    glm::mat4 globalTransformation = parentTransform * nodeTransform;

    auto boneInfoMap = m_currentAnimation->getBoneIDMap();
    glm::mat4 rootTransform = m_currentAnimation->getRootNode().transformation;
    
    if (boneInfoMap.find(nodeName) != boneInfoMap.end())
    {
        int index = boneInfoMap[nodeName].id;
        glm::mat4 offset = boneInfoMap[nodeName].offset;
        m_finalBoneMatrices[index] = globalTransformation * offset;
    }

    for (int i = 0; i < node->numChildren; i++)
        calculateBoneTransform(&node->children[i], globalTransformation);
}

