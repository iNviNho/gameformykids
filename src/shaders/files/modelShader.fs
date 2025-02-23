#version 330 core
out vec4 FragColor;

in vec2 TexCoords;
in vec3 aNormalPass;
in vec3 FragPos;

uniform sampler2D texture_diffuse1;

struct Light {
    vec3 position;
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};
uniform Light light;

void main()
{
    vec4 textureColor = texture(texture_diffuse1, TexCoords).rgba;

    if (textureColor.a < 0.5) {
        discard;
    }

    // ambient
    vec3 ambient = light.ambient * textureColor.rgb;

    // diffuse
    vec3 norm = normalize(aNormalPass);
    // light direction vector is the difference between the light position and the fragment position
    // this points from the fragment to the light source
    vec3 lightDir = normalize(light.position - FragPos);
    // dot product gives us the angle between the two vectors
    // the bigger the angle, the smaller the dot product
    // the smaller the angle, the bigger the dot product
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = diff * light.diffuse * textureColor.rgb;

    // TODO: specular

    FragColor = vec4(ambient + diffuse, textureColor.a);
}