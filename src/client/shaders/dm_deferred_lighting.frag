#version 330 core
out vec4 FragColor;

in vec2 TexCoords;

uniform sampler2D gPosition;
uniform sampler2D gNormal;
uniform sampler2D gAlbedoSpec;

struct DirLight {
    vec3 direction;

    vec3 ambient_color;
    vec3 diffuse_color;
    vec3 specular_color;
};

struct PointLight {
    // from 0-1
    float intensity;

    vec3 position;

    vec3 ambient_color;
    vec3 diffuse_color;
    vec3 specular_color;
    
    float attn_linear;
    float attn_quadratic;
};

#define NR_DIR_LIGHTS 4
uniform DirLight dirLights[NR_DIR_LIGHTS];

#define NR_POINT_LIGHTS 32
uniform PointLight pointLights[NR_POINT_LIGHTS];

uniform vec3 viewPos;

vec3 CalcPointLight(in PointLight light, in vec3 fragPos, in vec3 normal, in vec3 viewDir,
    vec3 mat_diffuse, float mat_shininess) {

    vec3 lightDir = normalize(light.position - fragPos);
    vec3 halfwayDir = normalize(lightDir + viewDir);  
    float diff = max(dot(normal, lightDir), 0.0);
    
    // vec3 ambient = light.ambient_color * mat_diffuse;
    vec3 diffuse = light.diffuse_color * diff * mat_diffuse;
    float spec = pow(max(dot(normal, halfwayDir), 0.0), 16.0);
    vec3 specular = light.specular_color * spec * mat_shininess;

    // attenuation
    float distance = length(light.position - fragPos);
    float attenuation = 1.0 / (1.0 + light.attn_linear * distance +
        light.attn_quadratic * (distance * distance));
    // ambient *= attenuation;
    diffuse *= attenuation;
    specular *= attenuation;
    return (diffuse + specular);        
}

vec3 CalcDirLight(in DirLight light, in vec3 normal, in vec3 viewDir,
    in vec3 mat_diffuse, in float mat_shininess) {
    vec3 lightDir = normalize(-light.direction);
    // diffuse shading
    float diff = max(dot(normal, lightDir), 0.0);
    // specular shading
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 16.0);
    // combine results
    vec3 ambient  = light.ambient_color  * mat_diffuse;
    vec3 diffuse  = light.diffuse_color  * diff * mat_diffuse;
    vec3 specular = light.specular_color * spec * mat_shininess;
    return (ambient + diffuse + specular);
}

void main() {             
    // retrieve data from gbuffer
    vec3 FragPos = texture(gPosition, TexCoords).rgb;
    vec3 Normal = normalize(texture(gNormal, TexCoords).rgb);
    vec3 Diffuse = texture(gAlbedoSpec, TexCoords).rgb;
    float Specular = texture(gAlbedoSpec, TexCoords).a;
    
    // then calculate lighting as usual
    vec3 viewDir  = normalize(viewPos - FragPos);
    vec3 result = vec3(0.0, 0.0, 0.0);
    for(int i = 0; i < NR_DIR_LIGHTS; ++i) {
        result += CalcDirLight(dirLights[i], Normal, viewDir, Diffuse, Specular);
    }
    for(int i = 0; i < NR_POINT_LIGHTS; ++i) {
        result += CalcPointLight(
            pointLights[i],
            FragPos,
            Normal,
            viewDir,
            Diffuse,
            Specular);
    }
    FragColor = vec4(result, 1.0);
}


