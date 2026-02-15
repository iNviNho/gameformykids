#ifndef MYGAME_ANIMATEDMODEL_H
#define MYGAME_ANIMATEDMODEL_H
#include "AbstractModel.h"
#include "BoneInformation.h"


class AnimatedModel : public AbstractModel {
public:
    explicit AnimatedModel(const std::filesystem::path& modelPath)
        : AbstractModel() {
    loadModel(modelPath);
    }

    const aiNodeAnim* FindNodeAnimation(const aiAnimation* animation, const std::string& string);
    unsigned int FindScaling(float animationTimeTicks, const aiNodeAnim *aiNodeAnimation);
    void CalcInterpolatedScaling(aiVector3D& scaling, float animationTime, const aiNodeAnim* aiNodeAnimation);
    unsigned int FindRotation(float animationTimeTicks, const aiNodeAnim* aiNodeAnimation);
    void CalcInterpolatedRotation(aiQuaternion& aiQuaternion, float animationTime, const aiNodeAnim* aiNodeAnimation);
    unsigned int FindPosition(float animationTimeTicks, const aiNodeAnim* aiNodeAnimation);
    void CalcInterpolatedPosition(aiVector3D& translation, float animationTime, const aiNodeAnim* aiNodeAnimation);
    void ReadNodeHierarchy(float animationTime, const aiNode* aiNode, glm::mat4 parentTransform);
    void GetBoneTransforms(float animationTimeInSec, std::vector<glm::mat4>& transforms);

    bool IsAnimated() const override { return true; }
protected:
    std::map<std::string, unsigned int> boneNameToIndexMap;
    std::vector<BoneInformation> vectorOfBones;
    int getBoneId(const aiBone* bone);

    Mesh processMesh(aiMesh *mesh, const aiScene *scene) override;

};





#endif //MYGAME_ANIMATEDMODEL_H