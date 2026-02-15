#ifndef MYGAME_VERTEXBONEDATA_H
#define MYGAME_VERTEXBONEDATA_H

#define MAX_NUM_BONES_PER_VERTEX 4

class VertexBoneData {
public:
    unsigned int BoneIDs[MAX_NUM_BONES_PER_VERTEX] = { 0 };
    float Weights[MAX_NUM_BONES_PER_VERTEX] = { 0.0f };

    void AddBoneData(unsigned int boneId, float mWeight);
};


#endif //MYGAME_VERTEXBONEDATA_H