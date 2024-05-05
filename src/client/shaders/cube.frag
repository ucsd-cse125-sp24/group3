#version 330 core

// Fragment shader of solid color, untextured cubes

in vec3 fragNormal;
in vec3 fragPos;

uniform vec3 AmbientColor = vec3(0.2);
uniform vec3 LightDirection = normalize(vec3(2, 4, 3));
uniform vec3 LightColor = vec3(1.0, 1.0, 1.0);
uniform vec3 DiffuseColor = vec3(1.0, 1.0, 1.0);

out vec4 fragColor;

void main() {
	// Compute irradiance (sum of ambient & direct lighting)
	vec3 irradiance = AmbientColor + LightColor * max(0, dot(LightDirection, fragNormal));

	// Diffuse reflectance
	vec3 reflectance = irradiance * DiffuseColor;

	// Gamma correction
	fragColor = vec4(sqrt(reflectance), 1);
}