#version 330 core

in vec2 TexCoords;

out vec4 FragColor;

uniform sampler2D textureImage;

void main()
{
    vec4 textureColor = texture(textureImage, TexCoords).rgba;
    if (textureColor.a < 0.5) {
        discard;
    }
    FragColor = textureColor;
}