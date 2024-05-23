#version 330 core

// Fragment shader of solid color, untextured cubes

in vec3 fragNormal;
in vec3 fragPos;

uniform vec3 AmbientColor = vec3(0.2);
uniform vec3 LightDirection = normalize(vec3(0, 1, 0));
uniform vec3 LightColor = vec3(1.0, 1.0, 1.0);
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
	vec3 irradiance = material.ambient + LightColor * max(0, dot(LightDirection, fragNormal));

	// Diffuse reflectance
	vec3 reflectance = irradiance * material.diffuse;

	// Gamma correction
	fragColor = vec4(sqrt(reflectance), 1);
}