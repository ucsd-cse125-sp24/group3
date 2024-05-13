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

out vec4 fragColor;

void main() {
    // ambient
    vec3 ambient = lightColor * material.ambient;
  	
    // diffuse 
    vec3 norm = normalize(fragNormal);
    vec3 lightDir = normalize(lightPos - fragPos);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = lightColor * (diff * material.diffuse);
    
    // specular
    vec3 viewDir = normalize(viewPos - fragPos);
    vec3 reflectDir = reflect(-lightDir, norm);  
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
    vec3 specular = lightColor * (spec * material.specular);  
        
    vec3 result = ambient + diffuse + specular;
    fragColor = vec4(result, 1.0);
}