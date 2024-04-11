#version 410 core

// Inputs to the fragment shader are the outputs of the same name from the vertex shader.
// Note that you do not have access to the vertex shader's default output, gl_Position.

in vec3 fragNormal;

// uniforms used for lighting
uniform vec3 AmbientColor = vec3(0.2);
uniform vec3 LightADirection = normalize(vec3(2, 4, 3));
//uniform vec3 LightAColor = vec3(0.9, 0.2, 0.5);
uniform vec3 LightAColor = vec3(0.0, 0.0, 0.0);
uniform vec3 LightBDirection = normalize(vec3(-2, 7, 3));
uniform vec3 LightBColor = vec3(0.2, 0.8, 0.6);
uniform vec3 DiffuseColor = vec3(1.0, 1.0, 1.0);

// You can output many things. The first vec4 type output determines the color of the fragment
out vec4 fragColor;

void main()
{

	// Compute irradiance (sum of ambient & direct lighting)
	vec3 irradiance = AmbientColor + LightAColor * max(0, dot(LightADirection, fragNormal)) + LightBColor * max(0, dot(LightBDirection, fragNormal));

	// Diffuse reflectance
	vec3 reflectance = irradiance * DiffuseColor;

	// Gamma correction
	fragColor = vec4(sqrt(reflectance), 1);
}