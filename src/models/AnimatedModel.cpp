#include "AnimatedModel.h"
#include "../utils/Log.h"
#include <algorithm>
#include <utility>

const aiNodeAnim* AnimatedModel::FindNodeAnimation(const aiAnimation *animation, const std::string &string) {

    for (unsigned int i = 0; i < animation->mNumChannels; i++) {
        const aiNodeAnim* nodeAnim = animation->mChannels[i];
        if (std::string(nodeAnim->mNodeName.C_Str()) == string) {
            return nodeAnim;
        }
    }
    return nullptr;
}



unsigned int AnimatedModel::FindScaling(
    float animationTimeTicks,
    const aiNodeAnim* pNodeAnimation
) {
    assert(pNodeAnimation->mNumScalingKeys > 0);

    for (uint i = 0; i < pNodeAnimation->mNumScalingKeys - 1; i++) {
        float t = static_cast<float>(
            pNodeAnimation->mScalingKeys[i + 1].mTime
        );

        if (animationTimeTicks < t) {
            return i;
        }
    }

    return pNodeAnimation->mNumScalingKeys - 2;
}


void AnimatedModel::CalcInterpolatedScaling(aiVector3D& out, float animationTimeTicks, const aiNodeAnim *pNodeAnimation) {
    // we need at least two values to interpolate...
    if (pNodeAnimation->mNumScalingKeys == 1) {
        out = pNodeAnimation->mScalingKeys[0].mValue;
        return;
    }

    uint ScalingIndex = FindScaling(animationTimeTicks, pNodeAnimation);
    uint NextScalingIndex = ScalingIndex + 1;
    assert(NextScalingIndex < pNodeAnimation->mNumScalingKeys);
    float t1 = (float)pNodeAnimation->mScalingKeys[ScalingIndex].mTime;
    float t2 = (float)pNodeAnimation->mScalingKeys[NextScalingIndex].mTime;
    float DeltaTime = t2 - t1;
    float Factor = (animationTimeTicks - (float)t1) / DeltaTime;
    assert(Factor >= 0.0f && Factor <= 1.0f);
    const aiVector3D& Start = pNodeAnimation->mScalingKeys[ScalingIndex].mValue;
    const aiVector3D& End   = pNodeAnimation->mScalingKeys[NextScalingIndex].mValue;
    aiVector3D Delta = End - Start;
    out = Start + Factor * Delta;
}

unsigned int AnimatedModel::FindRotation(
float animationTimeTicks,
const aiNodeAnim* pNodeAnimation
) {
assert(pNodeAnimation->mNumRotationKeys > 0);

for (uint i = 0; i < pNodeAnimation->mNumRotationKeys - 1; i++) {
    float t = static_cast<float>(
        pNodeAnimation->mRotationKeys[i + 1].mTime
    );

    if (animationTimeTicks < t) {
        return i;
    }
}

return pNodeAnimation->mNumRotationKeys - 2;
}

void AnimatedModel::CalcInterpolatedRotation(aiQuaternion& out, float animationTimeTicks, const aiNodeAnim *pNodeAnimation) {
    // we need at least two values to interpolate...
    if (pNodeAnimation->mNumRotationKeys == 1) {
        out = pNodeAnimation->mRotationKeys[0].mValue;
        return;
    }

    uint RotationIndex = FindRotation(animationTimeTicks, pNodeAnimation);
    uint NextRotationIndex = RotationIndex + 1;
    assert(NextRotationIndex < pNodeAnimation->mNumRotationKeys);
    float t1 = (float)pNodeAnimation->mRotationKeys[RotationIndex].mTime;
    float t2 = (float)pNodeAnimation->mRotationKeys[NextRotationIndex].mTime;
    float DeltaTime = t2 - t1;
    float Factor = (animationTimeTicks - t1) / DeltaTime;
    assert(Factor >= 0.0f && Factor <= 1.0f);
    const aiQuaternion& StartRotationQ = pNodeAnimation->mRotationKeys[RotationIndex].mValue;
    const aiQuaternion& EndRotationQ   = pNodeAnimation->mRotationKeys[NextRotationIndex].mValue;
    aiQuaternion::Interpolate(out, StartRotationQ, EndRotationQ, Factor);
    out.Normalize();
}

unsigned int AnimatedModel::FindPosition(
    float animationTimeTicks,
    const aiNodeAnim* pNodeAnimation
) {
    for (uint i = 0; i < pNodeAnimation->mNumPositionKeys - 1; i++) {
        float t = static_cast<float>(
            pNodeAnimation->mPositionKeys[i + 1].mTime
        );

        if (animationTimeTicks < t) {
            return i;
        }
    }

    return pNodeAnimation->mNumPositionKeys - 2;
}

void AnimatedModel::CalcInterpolatedPosition(aiVector3D& Out, float animationTimeTicks, const aiNodeAnim *pNodeAnimation) {
    // we need at least two values to interpolate...
    if (pNodeAnimation->mNumPositionKeys == 1) {
        Out = pNodeAnimation->mPositionKeys[0].mValue;
        return;
    }

    uint PositionIndex = FindPosition(animationTimeTicks, pNodeAnimation);
    uint NextPositionIndex = PositionIndex + 1;
    assert(NextPositionIndex < pNodeAnimation->mNumPositionKeys);
    float t1 = (float)pNodeAnimation->mPositionKeys[PositionIndex].mTime;
    float t2 = (float)pNodeAnimation->mPositionKeys[NextPositionIndex].mTime;
    float DeltaTime = t2 - t1;
    float Factor = (animationTimeTicks - t1) / DeltaTime;
    assert(Factor >= 0.0f && Factor <= 1.0f);
    const aiVector3D& Start = pNodeAnimation->mPositionKeys[PositionIndex].mValue;
    const aiVector3D& End = pNodeAnimation->mPositionKeys[NextPositionIndex].mValue;
    aiVector3D Delta = End - Start;
    Out = Start + Factor * Delta;
}

void AnimatedModel::ReadNodeHierarchy(float animationTime, const aiNode *aiNode, const glm::mat4 parentTransform) {
    std::string nodeName(aiNode->mName.C_Str());

    glm::mat4 nodeTransformation = glm::transpose(glm::make_mat4(&aiNode->mTransformation.a1));

    // Only process animations if they exist
    if (pScene->mNumAnimations > 0) {
        const aiAnimation* animation = pScene->mAnimations[animationIndex];

        const aiNodeAnim* pNodeAnimation = FindNodeAnimation(animation, nodeName);

        if (pNodeAnimation) {
            // Animation channels may contain only one or two of the S/R/T
            // tracks. Start from the node's bind pose and override only tracks
            // that actually have keys.
            aiVector3D scaling;
            aiQuaternion rotation;
            aiVector3D translation;
            aiNode->mTransformation.Decompose(scaling, rotation, translation);

            if (pNodeAnimation->mNumScalingKeys > 0) {
                CalcInterpolatedScaling(scaling, animationTime, pNodeAnimation);
            }
            glm::mat4 scalingM = glm::scale(glm::mat4(1.0f), glm::vec3(scaling.x, scaling.y, scaling.z));

            if (pNodeAnimation->mNumRotationKeys > 0) {
                CalcInterpolatedRotation(rotation, animationTime, pNodeAnimation);
            }
            glm::mat4 rotationM = glm::mat4_cast(glm::quat(rotation.w, rotation.x, rotation.y, rotation.z));

            if (pNodeAnimation->mNumPositionKeys > 0) {
                CalcInterpolatedPosition(translation, animationTime, pNodeAnimation);
            }
            glm::mat4 translationM = glm::translate(glm::mat4(1.0f), glm::vec3(translation.x, translation.y, translation.z));

            nodeTransformation = translationM * rotationM * scalingM;
        }
    }

    // we traverse from TOP to BOTTOM of the hierarchy
    glm::mat4 globalTransformation = parentTransform * nodeTransformation;

    if (boneNameToIndexMap.find(nodeName) != boneNameToIndexMap.end()) {
        int boneIndex = boneNameToIndexMap[nodeName];
        // back to the root
        vectorOfBones[boneIndex].finalTransformation = globalInverseTransform * globalTransformation * vectorOfBones[boneIndex].offsetMatrix;
    }

    // recursively on its child node
    for (unsigned int i = 0; i < aiNode->mNumChildren; i++) {
        ReadNodeHierarchy(animationTime, aiNode->mChildren[i], globalTransformation);
    }
}

void AnimatedModel::GetBoneTransforms(float animationTimeInSec, std::vector<glm::mat4> &transforms) {
    globalInverseTransform =
    glm::inverse(glm::transpose(
        glm::make_mat4(&pScene->mRootNode->mTransformation.a1)
    ));
    transforms.resize(vectorOfBones.size());

    glm::mat4 identity = glm::mat4(1.0f);

    float animationTime = 0.0f;
    if (pScene->mNumAnimations > 0) {
        const aiAnimation* anim = pScene->mAnimations[animationIndex];
        float ticksPerSecond = anim->mTicksPerSecond != 0 ? anim->mTicksPerSecond : 25.0f;
        float timeInTicks = animationTimeInSec * ticksPerSecond;
        animationTime = fmod(timeInTicks, static_cast<float>(anim->mDuration));
    }

    ReadNodeHierarchy(animationTime, pScene->mRootNode, identity);

    for (size_t i = 0; i < vectorOfBones.size(); i++) {
        transforms[i] = vectorOfBones[i].finalTransformation;
    }
}

unsigned int AnimatedModel::GetAnimationCount() const {
    return pScene ? pScene->mNumAnimations : 0;
}

void AnimatedModel::logAnimations() const {
    if (!pScene || pScene->mNumAnimations == 0) {
        Log::logInfo("Model has no animations.");
        return;
    }
    Log::logInfo("Model has " + std::to_string(pScene->mNumAnimations) + " animation(s):");
    for (unsigned int i = 0; i < pScene->mNumAnimations; i++) {
        const aiAnimation* anim = pScene->mAnimations[i];
        Log::logInfo("  [" + std::to_string(i) + "] \"" + std::string(anim->mName.C_Str()) +
                     "\" duration=" + std::to_string(anim->mDuration) +
                     " ticks, ticksPerSec=" + std::to_string(anim->mTicksPerSecond));
    }
}

int AnimatedModel::getBoneId(const aiBone* bone) {
    std::string boneName(bone->mName.C_Str());
    if (boneNameToIndexMap.find(boneName) == boneNameToIndexMap.end()) {
        // we create unique sequential bone ID for each new bone encountered
        // First bone: size() = 0 → newIndex = 0
        // Second bone: size() = 1 → newIndex = 1
        // Third bone: size() = 2 → newIndex = 2
        int newIndex = static_cast<int>(boneNameToIndexMap.size());
        boneNameToIndexMap[boneName] = newIndex;
        return newIndex;
    }
    return boneNameToIndexMap[boneName];
}

Mesh AnimatedModel::processMesh(aiMesh* mesh, const aiScene* scene) {
    Mesh result = AbstractModel::processMesh(mesh, scene);
Log::logInfo(
    "Mesh: " + std::string(mesh->mName.C_Str()) +
    ", vertices=" + std::to_string(mesh->mNumVertices) +
    ", bones=" + std::to_string(mesh->mNumBones)
);
    // bones parsing
    std::vector<VertexBoneData> boneDataPerVertex;
    if (mesh->HasBones()) {
        // Collect every influence first so we can select the strongest four,
        // independent of the order in which Assimp lists bones.
        using BoneInfluence = std::pair<unsigned int, float>;
        std::vector<std::vector<BoneInfluence>> influencesPerVertex(mesh->mNumVertices);

        // we loop through all bones of this mesh
        for (int i = 0; i < mesh->mNumBones; i++) {
            aiBone* bone = mesh->mBones[i];

            // first we need to get bone id
            int boneId = getBoneId(bone);

            // store the offset matrix
            if (boneId == vectorOfBones.size()) {
                // Convert aiMatrix4x4 to glm::mat4
                glm::mat4 offsetMatrix = glm::transpose(glm::make_mat4(&bone->mOffsetMatrix.a1));
                vectorOfBones.push_back(BoneInformation(offsetMatrix));
            }

            // we read the weights for each vertex assigned to this bone
            for (unsigned int j = 0; j < bone->mNumWeights; j++) {
                aiVertexWeight& weight = bone->mWeights[j];

                unsigned int localVertexId = weight.mVertexId;
                if (localVertexId < influencesPerVertex.size() && weight.mWeight > 0.0f) {
                    influencesPerVertex[localVertexId].emplace_back(
                        static_cast<unsigned int>(boneId), weight.mWeight
                    );
                }
            }
        }

        boneDataPerVertex.resize(influencesPerVertex.size());
        for (size_t vertexIndex = 0; vertexIndex < influencesPerVertex.size(); ++vertexIndex) {
            auto& influences = influencesPerVertex[vertexIndex];
            std::sort(influences.begin(), influences.end(),
                      [](const BoneInfluence& left, const BoneInfluence& right) {
                          return left.second > right.second;
                      });

            const size_t influenceCount = std::min<size_t>(4, influences.size());
            float totalWeight = 0.0f;
            for (size_t i = 0; i < influenceCount; ++i) {
                totalWeight += influences[i].second;
            }

            if (totalWeight <= 0.0f) {
                continue;
            }

            for (size_t i = 0; i < influenceCount; ++i) {
                boneDataPerVertex[vertexIndex].BoneIDs[i] = influences[i].first;
                boneDataPerVertex[vertexIndex].Weights[i] = influences[i].second / totalWeight;
            }
        }
    }
size_t weightedVertices = 0;

for (const auto& vertex : boneDataPerVertex) {
    float totalWeight = 0.0f;

    for (int i = 0; i < 4; i++) {
        totalWeight += vertex.Weights[i];
    }

    if (totalWeight > 0.0f) {
        weightedVertices++;
    }
}

Log::logInfo(
    "Weighted vertices: " +
    std::to_string(weightedVertices) + "/" +
    std::to_string(boneDataPerVertex.size())
);

    return Mesh(
        std::move(result.getVertices()),
        std::move(result.GetIndices()),
        std::move(result.getTextures()),
        std::move(boneDataPerVertex)
    );
}
