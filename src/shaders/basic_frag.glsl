#version 330 core

struct Material 
{
    vec3 diffuse;
    vec3 specular;
    float shininess;
};

struct DirectionalLight 
{
    vec3 ambient;
    vec3 direction;
    vec3 diffuse;
    vec3 specular;
};

in vec3 normal;
in vec3 fragPos;

out vec4 fragColor;

uniform Material material;
uniform DirectionalLight dirLight;
uniform vec3 viewPos;

void main()
{
    vec3 normal = normalize(normal);
    vec3 viewDir = normalize(viewPos - fragPos);
    vec3 lightDir = normalize(-dirLight.direction);
    vec3 halfwayDir = normalize(lightDir + viewDir);
    //vec3 reflectDir = reflect(-lightDir, normal);
    float diff = max(dot(normal, lightDir), 0.0);
    float spec = pow(max(dot(normal, halfwayDir), 0.0), material.shininess);

    vec3 ambient = dirLight.ambient * material.diffuse;
    vec3 diffuse = dirLight.diffuse * diff * material.diffuse;
    vec3 specular = dirLight.specular * spec * material.specular;

    fragColor = vec4(ambient + diffuse + specular, 1.0);
}