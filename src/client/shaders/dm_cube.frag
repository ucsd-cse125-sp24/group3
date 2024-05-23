#version 330 core

// Fragment shader of solid color, untextured cubes

in vec3 fragNormal;
in vec3 fragPos;
in vec2 TexCoords;

uniform vec3 AmbientColor = vec3(0.2);
uniform vec3 LightDirection = normalize(vec3(2, 4, 3));
uniform vec3 LightColor = vec3(0.2, 0.2, 0.2);
uniform vec3 DiffuseColor = vec3(1.0, 1.0, 1.0);

struct Material {
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    float shininess;
    sampler2D texture_diffuse1;
}; 

uniform Material material;

out vec4 fragColor;

void main() {
	// Compute irradiance (sum of ambient & direct lighting)
	vec3 irradiance = vec3(texture(material.texture_diffuse1, TexCoords)) + LightColor * max(0, dot(LightDirection, fragNormal));

	// Diffuse reflectance
	vec3 reflectance = irradiance * vec3(texture(material.texture_diffuse1, TexCoords));

	// Gamma correction
	fragColor = vec4(sqrt(reflectance), 1);
}