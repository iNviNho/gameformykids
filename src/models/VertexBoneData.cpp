#include "VertexBoneData.h"

#include "../utils/Log.h"
#include "glm/ext/vector_uint3.hpp"
#include <string>

void VertexBoneData::AddBoneData(unsigned int boneId, float weight) {
    // Log::logInfo("Adding boneId: " + std::to_string(boneId) + "with weight:" + std::to_string(weight));
    for (unsigned int i = 0; i < MAX_NUM_BONES_PER_VERTEX; i++) {
        if (Weights[i] == 0.0) {
            BoneIDs[i] = boneId;
            Weights[i] = weight;
            return;
        }
    }

    // should never get here - more bones than we have space for
    assert(0);
}
