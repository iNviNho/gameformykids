#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoords;
layout (location = 3) in ivec4 boneIDs;
layout (location = 4) in vec4 weights;

out vec2 TexCoords;
out vec3 aNormalPass;
out vec3 FragPos;
flat out ivec4 BoneIDsPass;
out vec4 WeightsPass;

const int MAX_BONES = 100;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform mat4 gBones[MAX_BONES];

void main()
{
    TexCoords = aTexCoords;
    aNormalPass = aNormal;
    FragPos = vec3(model * vec4(aPos, 1.0));
    BoneIDsPass = boneIDs;
    WeightsPass = weights;

    // Check if this vertex has valid bone data
    // Rigged models: at least one of first 3 weights is non-zero
    // Non-rigged models: default weights are (0, 0, 0, 1), so first 3 are zero
    if (weights[0] > 0.0 || weights[1] > 0.0 || weights[2] > 0.0) {
        // Has bone data - apply bone transformation
        mat4 BoneTransform = gBones[boneIDs[0]] * weights[0];
        BoneTransform += gBones[boneIDs[1]] * weights[1];
        BoneTransform += gBones[boneIDs[2]] * weights[2];
        BoneTransform += gBones[boneIDs[3]] * weights[3];
        gl_Position = projection * view * model * BoneTransform * vec4(aPos, 1.0);
    } else {
        // No bone data - use vertex position directly
        gl_Position = projection * view * model * vec4(aPos, 1.0);
    }
}