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

in VS_OUT {
    vec3 fragPos;
    vec3 normal;
    vec4 fragPosLightSpace;
} fs_in;

out vec4 fragColor;

uniform Material material;
uniform DirectionalLight dirLight;
uniform vec3 viewPos;
uniform sampler2D shadowMap;

float shadowCalculation(vec4 fragPosLightSpace, vec3 lightDir)
{
    vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
    projCoords = projCoords * 0.5 + 0.5;
    float closestDepth = texture(shadowMap, projCoords.xy).r;
    float currentDepth = projCoords.z;

    
    float shadow = 0.0;
    float bias = max(0.05 * (1.0 - dot(fs_in.normal, lightDir)), 0.005);
    if (projCoords.z > 1.0) {
        shadow = 0.0;
    } else {
        vec2 texelSize = 1.0 / textureSize(shadowMap, 0);
        for (int x = -1; x <= 1; x++) {
            for (int y = -1; y <= 1; y++) {
                float pcfDepth = texture(shadowMap, projCoords.xy + vec2(x, y) * texelSize).r;
                shadow += currentDepth - bias > pcfDepth? 1.0: 0.0;
            }
        }
        shadow /= 9.0;
    }
    return shadow;
}

void main()
{
    vec3 normal = normalize(fs_in.normal);
    vec3 viewDir = normalize(viewPos - fs_in.fragPos);
    vec3 lightDir = normalize(-dirLight.direction);
    vec3 halfwayDir = normalize(lightDir + viewDir);
    //vec3 reflectDir = reflect(-lightDir, normal);
    float diff = max(dot(normal, lightDir), 0.0);
    float spec = pow(max(dot(normal, halfwayDir), 0.0), material.shininess);

    float shadow = shadowCalculation(fs_in.fragPosLightSpace, lightDir);
    vec3 ambient = dirLight.ambient * material.diffuse;
    vec3 diffuse = dirLight.diffuse * diff * material.diffuse;
    vec3 specular = dirLight.specular * spec * material.specular;

    fragColor = vec4(ambient + (1.0 - shadow) * (diffuse + specular), 1.0);
}