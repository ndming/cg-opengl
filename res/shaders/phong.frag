#version 440 core

in vec3 fragPosition;
in vec3 fragNormal;

out vec4 FragColor;

struct Material {
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    float shininess;
};

struct DirectionalLight {
    vec3 direction;
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

struct PointLight {
    vec3 position;
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    float constant;
    float linear;
    float quadratic;
};

uniform Material material;

uniform DirectionalLight directionalLight;
uniform bool enabledDirectionalLight;

uniform PointLight pointLight;
uniform bool enabledPointLight;

vec3 calcDirLight(DirectionalLight light, vec3 normal, vec3 toView);
vec3 calcPointLight(PointLight light, vec3 normal, vec3 toView);

void main() {
    vec3 norm = normalize(fragNormal);
    vec3 toView = normalize(-fragPosition);
    
    vec3 shading = vec3(0.0f);
    if (enabledDirectionalLight) {
        shading += calcDirLight(directionalLight, norm, toView);
    }
    if (enabledPointLight) {
        shading += calcPointLight(pointLight, norm, toView);
    }

	FragColor = vec4(shading, 1.0f);
}

vec3 calcDirLight(DirectionalLight light, vec3 normal, vec3 toView) {
    // Ambient
    vec3 ambient = light.ambient * material.ambient;
    // Diffuse
    vec3 toLight = normalize(-light.direction);
    float diff = max(dot(normal, toLight), 0.0);
    vec3 diffuse = light.diffuse * (diff * material.diffuse);
    // Specular
    vec3 reflectDir = reflect(-toLight, normal);
    float spec = pow(max(dot(reflectDir, toView), 0.0f), material.shininess);
    vec3 specular = light.specular * (spec * material.specular);

    return ambient + diffuse + specular;
}

vec3 calcPointLight(PointLight light, vec3 normal, vec3 toView) {
    // Ambient
    vec3 ambient = light.ambient * material.ambient;
    // Diffuse
    vec3 toLight = normalize(light.position - fragPosition);
    float diff = max(dot(normal, toLight), 0.0f);
    vec3 diffuse = light.diffuse * (diff * material.diffuse);
    // Specular
    vec3 reflectDir = reflect(-toLight, normal);
    float spec = pow(max(dot(reflectDir, toView), 0.0f), material.shininess);
    vec3 specular = light.specular * (spec * material.specular);

    // Attenuation
    float dist = length(light.position - fragPosition);
    float attenuation = 1.0f / (light.constant + light.linear * dist + light.quadratic * (dist * dist));
    ambient  *= attenuation;
    diffuse  *= attenuation;
    specular *= attenuation;

    return ambient + diffuse + specular;
}