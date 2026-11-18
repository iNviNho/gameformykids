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

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform bool meshHasBones;

layout (std140) uniform BoneMatrices {
    mat4 gBones[142];
};

void main()
{
    TexCoords = aTexCoords;
    aNormalPass = aNormal;
    FragPos = vec3(model * vec4(aPos, 1.0));
    BoneIDsPass = boneIDs;
    WeightsPass = weights;

    // Disabled vertex attributes have OpenGL default values, including a
    // fourth component of 1.0. Do not infer whether this mesh is rigged from
    // the weights; use the mesh-level flag set by Mesh::Draw instead.
    if (meshHasBones) {
        mat4 BoneTransform = gBones[boneIDs[0]] * weights[0];
        BoneTransform += gBones[boneIDs[1]] * weights[1];
        BoneTransform += gBones[boneIDs[2]] * weights[2];
        BoneTransform += gBones[boneIDs[3]] * weights[3];
        gl_Position = projection * view * model * BoneTransform * vec4(aPos, 1.0);
    } else {
        gl_Position = projection * view * model * vec4(aPos, 1.0);
    }
}
