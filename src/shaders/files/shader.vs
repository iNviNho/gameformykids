#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoord;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform vec3 lightPos;

out vec2 TexCoord;
out vec3 Normal;
out vec3 FragPos;

void main()
{
    // we want a vertex position that is in world space first
    // We can accomplish this by multiplying the vertex position attribute with the model matrix only (not the view
    // and projection matrix) to transform it to world space coordinates.
    FragPos = vec3(model * vec4(aPos, 1.0));
    // expensive, do inverse in app (CPU)
    Normal = mat3(transpose(inverse(model))) * aNormal;
    TexCoord = aTexCoord;

    gl_Position = projection * view * model * vec4(FragPos, 1.0);

}