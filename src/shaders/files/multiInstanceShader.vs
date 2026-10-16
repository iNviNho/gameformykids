#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoords;
layout (location = 3) in mat4 aInstanceMatrix;
layout (location = 7) in ivec4 boneIDs;
layout (location = 8) in vec4 weights;

out vec2 TexCoords;
out vec3 aNormalPass;
out vec3 FragPos;
flat out ivec4 BoneIDsPass;
out vec4 WeightsPass;

uniform mat4 view;
uniform mat4 projection;

void main()
{
    TexCoords = aTexCoords;
    aNormalPass = aNormal;
    FragPos = vec3(aInstanceMatrix * vec4(aPos, 1.0));
    gl_Position = projection * view * aInstanceMatrix * vec4(aPos, 1.0);
    BoneIDsPass = boneIDs;
    WeightsPass = weights;
}