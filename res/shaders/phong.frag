#version 440 core

in vec3 fragPosition;
in vec3 fragNormal;
in vec2 fragUV0;

out vec4 FragColor;

struct Material {
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    float shininess;
};

struct TexturedMaterial {
    sampler2D diffuse;
    sampler2D specular;
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

uniform TexturedMaterial texturedMaterial;
uniform bool enabledTexturedMaterial;

uniform DirectionalLight directionalLight;
uniform bool enabledDirectionalLight;

uniform PointLight pointLights[5];
uniform int pointLightCount;

vec3 calcDirLight(vec3 normal, vec3 toView);
vec3 calcPointLight(PointLight pointLight, vec3 normal, vec3 toView);

void main() {
    vec3 norm = normalize(fragNormal);
    vec3 toView = normalize(-fragPosition);

    vec3 shading = vec3(0.0);
    if (enabledDirectionalLight) {
        shading += calcDirLight(norm, toView);
    }

    for (int i = 0; i < pointLightCount; i++) {
        shading += calcPointLight(pointLights[i], norm, toView);
    }

    FragColor = vec4(shading, 1.0);
}

vec3 calcDirLight(vec3 normal, vec3 toView) {
    // Ambient
    vec3 ambient = directionalLight.ambient;
    if (enabledTexturedMaterial) {
        // Texture ambient should be the same as texture diffuse
        ambient *= vec3(texture(texturedMaterial.diffuse, fragUV0));
    } else {
        ambient *= material.ambient;
    }
    // Diffuse
    vec3 toLight = normalize(-directionalLight.direction);
    float diff = max(dot(normal, toLight), 0.0);
    vec3 diffuse = directionalLight.diffuse * diff;
    if (enabledTexturedMaterial) {
        diffuse *= vec3(texture(texturedMaterial.diffuse, fragUV0));
    } else {
        diffuse *= material.diffuse;
    }
    // Specular
    vec3 reflectDir = reflect(-toLight, normal);
    vec3 specular = directionalLight.specular;
    if (enabledTexturedMaterial) {
        float spec = pow(max(dot(reflectDir, toView), 0.0), texturedMaterial.shininess);
        specular *= (spec * vec3(texture(texturedMaterial.specular, fragUV0)));
    } else {
        float spec = pow(max(dot(reflectDir, toView), 0.0), material.shininess);
        specular *= (spec * material.specular);
    }

    return ambient + diffuse + specular;
}

vec3 calcPointLight(PointLight pointLight, vec3 normal, vec3 toView) {
    // Ambient
    vec3 ambient = pointLight.ambient;
    if (enabledTexturedMaterial) {
        // Texture ambient should be the same as texture diffuse
        ambient *= vec3(texture(texturedMaterial.diffuse, fragUV0));
    } else {
        ambient *= material.ambient;
    }
    // Diffuse
    vec3 toLight = normalize(pointLight.position - fragPosition);
    float diff = max(dot(normal, toLight), 0.0);
    vec3 diffuse = pointLight.diffuse * diff;
    if (enabledTexturedMaterial) {
        diffuse *= vec3(texture(texturedMaterial.diffuse, fragUV0));
    } else {
        diffuse *= material.diffuse;
    }
    // Specular
    vec3 reflectDir = reflect(-toLight, normal);
    vec3 specular = pointLight.specular;
    if (enabledTexturedMaterial) {
        float spec = pow(max(dot(reflectDir, toView), 0.0), texturedMaterial.shininess);
        specular *= spec * vec3(texture(texturedMaterial.specular, fragUV0));
    } else {
        float spec = pow(max(dot(reflectDir, toView), 0.0), material.shininess);
        specular *= (spec * material.specular);
    }

    // Attenuation
    float dist = length(pointLight.position - fragPosition);
    float attenuation = 1.0 / (pointLight.constant + pointLight.linear * dist + pointLight.quadratic * (dist * dist));
    ambient *= attenuation;
    diffuse *= attenuation;
    specular *= attenuation;

    return ambient + diffuse + specular;
}
