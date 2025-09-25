#version 330 core

in vec2 TexCoords;

out vec4 FragColor;

uniform sampler2D textureImage;

void main()
{
    FragColor = texture(textureImage, TexCoords).rgba;
}