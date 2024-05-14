#version 330 core

// Fragment shader for loaded models.
// This shader currently expects textured models. Untextured
// models will show up as black.

in vec3 fragNormal;
in vec3 fragPos;
in vec2 TexCoords;

struct Material {
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    float shininess;
    sampler2D texture_diffuse1;
}; 

uniform Material material;
uniform vec3 viewPos;

uniform vec3 lightPos;
uniform vec3 lightColor;

struct PointLight {    
    vec3 position;
    
    float constant;
    float linear;
    float quadratic;  

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;

    // from 0-1
    float intensity;
};

#define NR_POINT_LIGHTS 1
uniform PointLight pointLights[NR_POINT_LIGHTS];

out vec4 fragColor;

vec3 CalcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir) {
    vec3 lightDir = normalize(light.position - fragPos);
    vec3 halfwayDir = normalize(lightDir + viewDir);
    // diffuse shading
    float diff = max(dot(normal, lightDir), 0.0);
    // specular shading
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(normal, halfwayDir), 0.0), material.shininess);
    // attenuation
    float distance    = length(light.position - fragPos);
    float attenuation = 1.0 / (light.constant + light.linear * distance + 
  			     light.quadratic * (distance * distance));    
    // combine results 
    // TODO: put textures here once we want textured surfaces
    vec3 ambient  = light.ambient  * material.ambient;
    vec3 diffuse  = light.diffuse  * diff * material.diffuse;
    vec3 specular = light.specular * spec;
    ambient  *= attenuation;
    diffuse  *= attenuation;
    specular *= attenuation;
    return (ambient + diffuse + specular) * light.intensity;
}

void main() {
    vec3 norm = normalize(fragNormal);
    vec3 viewDir = normalize(viewPos - fragPos);

    vec3 result = vec3(0.0, 0.0, 0.0);
    for(int i = 0; i < NR_POINT_LIGHTS; i++)
        result += CalcPointLight(pointLights[i], norm, fragPos, viewDir);
    fragColor = vec4(result, 1.0);
}
