#version 330 core
out vec4 FragColor;

in vec2 TexCoord;
in vec3 aPosPass;
in vec3 aNormalPass;
in vec3 FragPos;

uniform sampler2D mud;
uniform sampler2D path;
uniform sampler2D grass;
uniform sampler2D flowers;
uniform sampler2D blendMap;

uniform float terrainSize;

uniform vec2 mouseCoord;
uniform bool editTerrainCircle;
uniform float editTerrainCircleRadius;

struct Light {
    vec3 position;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};
uniform Light light;

void main()
{
    
    vec2 coord = vec2(aPosPass.x / terrainSize, -aPosPass.z / terrainSize);
    vec3 blendMapColour = texture(blendMap, coord).rgb;
    vec3 mudColor = texture(mud, TexCoord).rgb * blendMapColour.r;
    vec3 flowersColor = texture(flowers, TexCoord).rgb * blendMapColour.g;
    vec3 pathColor = texture(path, TexCoord).rgb * blendMapColour.b;
    vec3 grassColor = texture(grass, TexCoord).rgb * (1 - (blendMapColour.r + blendMapColour.g + blendMapColour.b));

    vec3 finalColor = grassColor + mudColor + pathColor + flowersColor;

    // ambient
    vec3 ambient = light.ambient * finalColor;

    // diffuse
    vec3 norm = normalize(aNormalPass);
    // light direction vector is the difference between the light position and the fragment position
    // this points from the fragment to the light source
    vec3 lightDir = normalize(light.position - FragPos);
    // dot product gives us the angle between the two vectors
    // the bigger the angle, the smaller the dot product
    // the smaller the angle, the bigger the dot product
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = diff * light.diffuse * finalColor.xyz;

    // TODO: specular

    // game edit mode to draw circle where the terrain would be modified
    vec4 overlayColor = vec4(1.0f, 0.0f, 0.0f, 1);
    float mask = 0.0f;
    if (editTerrainCircle) {

        vec2 delta = mouseCoord.xy - aPosPass.xz;
        float distSq = dot(delta, delta);
        mask = distSq < editTerrainCircleRadius * editTerrainCircleRadius ? 0.1f : 0.0f;
    }

    vec4 baseColor = vec4(ambient + diffuse, 1.0);
    FragColor = mix(baseColor, overlayColor, mask);
}
