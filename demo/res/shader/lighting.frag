#version 330 core

#define N_MAX_LIGHTS 256

#define LIGHT_TYPE_DIR 0u
#define LIGHT_TYPE_POINT 1u
#define LIGHT_TYPE_SPOT 2u

const float ambientStrength = 1.0;
const float diffuseStrength = 1.0;
const float specularStrength = 1.0;

struct Light {
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;

    vec3 position;
    vec3 direction;
    uint type;

    // Point
    float constant;
    float linear;
    float quadratic;

    // Spot
    float cutOff;
    float outerCutOff;
};

uniform Light lights[N_MAX_LIGHTS];
uniform uint nLights;

uniform vec3 viewPos;

out vec4 FragColor;

vec3 FragPos;
vec3 FragNorm;
in vec2 FragTex;
vec3 FragAlbedo;
float FragShine;

uniform sampler2D gPosition;
uniform sampler2D gNormal;
uniform sampler2D gAlbedo;

vec3 calcDirLight(Light light, vec3 normal, vec3 viewDir) {
    vec3 lightDir = normalize(light.position - FragPos);

    // diffuse shading
    float diff = max(dot(normal, lightDir), 0.0);

    // specular shading
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), FragShine);

    // combine results
    vec3 ambient = light.ambient * FragAlbedo * ambientStrength;
    vec3 diffuse = light.diffuse * FragAlbedo * diff * diffuseStrength;
    vec3 specular = light.specular * FragShine * spec * diffuseStrength;

    return (ambient + diffuse + specular);
}

vec3 calcPointLight(Light light, vec3 normal, vec3 viewDir)
{
    vec3 color = calcDirLight(light, normal, viewDir);

    // attenuation
    float distance = length(light.position - FragPos);
    float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));    

    return color * attenuation;
}

vec3 calcSpotLight(Light light, vec3 normal, vec3 viewDir)
{
    vec3 lightDir = normalize(light.position - FragPos);

    // vec3 color = calcPointLight(light, normal, viewDir);
    vec3 color = calcDirLight(light, normal, viewDir);

    // spotlight intensity
    float theta = dot(lightDir, normalize(-light.direction)); 
    float epsilon = light.cutOff - light.outerCutOff;
    float intensity = clamp((theta - light.outerCutOff) / epsilon, 0.0, 1.0);

    return color * intensity;
}

vec3 calcLight(Light light, vec3 normal, vec3 viewDir) {
    if (light.type == LIGHT_TYPE_DIR)
        return calcDirLight(light, normal, viewDir);
    else if (light.type == LIGHT_TYPE_POINT)
        return calcPointLight(light, normal, viewDir);
    else if (light.type == LIGHT_TYPE_SPOT)
        return calcSpotLight(light, normal, viewDir);
    else
        return vec3(0.0, 1.0, 0.0);
}

vec3 tonemap(vec3 color)
{
    const float gamma = 2.2;
    vec3 mapped = color / (color + vec3(1.0));
    return pow(mapped, vec3(1.0 / gamma));
}

void main()
{
    FragPos = texture(gPosition, FragTex).rgb;
    FragNorm = texture(gNormal, FragTex).rgb;
    FragAlbedo = texture(gAlbedo, FragTex).rgb;
    FragShine = texture(gAlbedo, FragTex).a;

    vec3 normal = normalize(FragNorm);
    vec3 viewDir = normalize(viewPos - FragPos);

    vec3 color = vec3(0.0);
    for (uint i = 0u; i < nLights; i++) {
        color += calcLight(lights[i], normal, viewDir);
    }

    FragColor = vec4(tonemap(color), 1.0);
    //FragColor = texture(gAlbedo, FragTex);
    
    //if (color.r >= 1.0 || color.g >= 1.0 || color.b >= 1.0)
    //    color = vec3(1.0, 0.0, 0.0);
    //if (color.r < 0.0 || color.g < 0.0 || color.b < 0.0)
    //    color = vec3(0.0, 0.0, 1.0);

    //FragColor = vec4(color, 1.0);
}

