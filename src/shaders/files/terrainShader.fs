#version 330 core
out vec4 FragColor;

in vec2 TexCoord;
in vec3 aPosPass;

uniform sampler2D mud;
uniform sampler2D path;
uniform sampler2D grass;
uniform sampler2D flowers;
uniform sampler2D blendMap;

uniform float terrainSize;

void main()
{
    vec2 coord = vec2(aPosPass.x / terrainSize, aPosPass.z / terrainSize);
    vec4 blendMapColour = texture(blendMap, coord);
    vec4 mudColor = texture(mud, TexCoord) * blendMapColour.r;
    vec4 flowersColor = texture(flowers, TexCoord) * blendMapColour.g;
    vec4 pathColor = texture(path, TexCoord) * blendMapColour.b;
    vec4 grassColor = texture(grass, TexCoord) * (1 - (blendMapColour.r + blendMapColour.g + blendMapColour.b));

    FragColor = grassColor + mudColor + pathColor + flowersColor;
}