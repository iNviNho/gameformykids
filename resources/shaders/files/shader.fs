#version 330 core
out vec4 FragColor;

struct Material {
    sampler2D diffuse;
    vec3 specular;
    float shininess;
};
struct Light {
    vec3 position;
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

in vec2 TexCoord;
in vec3 Normal;
in vec3 FragPos;

uniform vec3 viewPos;

uniform Material material;
uniform Light light;

void main()
{
    // ambient
    vec3 ambient = light.ambient * texture(material.diffuse, TexCoord).rgb;

    // diffuse
    vec3 norm = normalize(Normal);
    // light direction vector is the difference between the light position and the fragment position
    // this points from the fragment to the light source
    vec3 lightDir = normalize(light.position - FragPos);
    // dot product gives us the angle between the two vectors
    // the bigger the angle, the smaller the dot product
    // the smaller the angle, the bigger the dot product
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = diff * light.diffuse * texture(material.diffuse, TexCoord).rgb;

    // specular
    vec3 viewDir = normalize(viewPos - FragPos);
    // you reverse direction by negating the light direction
    vec3 reflectDir = reflect(-lightDir, norm);
    // the higher the shininess, the smaller the specular highlight
    // in another words, the higher the number, the more correct circle will be
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
    vec3 specular = material.specular * spec * light.specular;

    vec3 result = (ambient + diffuse + specular);

    FragColor = vec4(result, 1.0);
}