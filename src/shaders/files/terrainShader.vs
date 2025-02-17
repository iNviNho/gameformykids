#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aTexCoord;
layout (location = 2) in vec3 aNormal;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

out vec2 TexCoord;
out vec3 aPosPass;
out vec3 aNormalPass;
out vec3 FragPos;

void main()
{
    TexCoord = aTexCoord;
    aPosPass = aPos;
    aNormalPass = aNormal;
    // we want a vertex position that is in world space first
    // We can accomplish this by multiplying the vertex position attribute with the model matrix only (not the view
    // and projection matrix) to transform it to world space coordinates.
    FragPos = vec3(model * vec4(aPos, 1.0));
    gl_Position = projection * view * model * vec4(aPos, 1.0);
}