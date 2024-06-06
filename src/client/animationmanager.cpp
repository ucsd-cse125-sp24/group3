#include "client/animationmanager.hpp"

AnimationManager::AnimationManager(Animation* animation) {
    currEntity = 0;
    m_currentTime = 0.0;
    m_currentAnimation = animation;

    m_deltaTime = 0; // Tyler: is this still used? linter complaining about it not being initialized

    m_finalBoneMatrices.reserve(100);

    for (int i = 0; i < 100; i++)
        m_finalBoneMatrices.push_back(glm::mat4(1.0f));
}

void AnimationManager::updateAnimation(float dt) {
    m_deltaTime = dt;
    if (entityAnimMap[currEntity].second) {
        m_currentAnimation = entityAnimMap[currEntity].second;
        m_currentTime = entityAnimMap[currEntity].first;
        m_currentTime += m_currentAnimation->getTicksPerSecond() * dt;
        m_currentTime = fmod(m_currentTime, m_currentAnimation->getDuration());
        entityAnimMap[currEntity].first = m_currentTime;
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

void AnimationManager::setAnimation(EntityID id, ObjectType objType, AnimState animState) {
    if (entityAnimMap.find(id) == entityAnimMap.end() || entityAnimMap[id].second != objAnimMap[objType][animState]) {
        entityAnimMap[id] = std::make_pair(0.0f, objAnimMap[objType][animState]);
    }
    currEntity = id;
}

void AnimationManager::addAnimation(Animation* anim, ObjectType objType, AnimState animState) {
    if (objAnimMap.find(objType) == objAnimMap.end()) {
        std::unordered_map<AnimState, Animation*> animMap;
        objAnimMap[objType] = animMap;
    }

    this->objAnimMap[objType][animState] = anim;
}

