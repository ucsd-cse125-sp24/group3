#version 330 core
#
// Vertex shader for loaded models.
// Also forwards texture coordinates to fragment shader.

layout (location = 0) in vec3 position;
layout (location = 1) in vec3 normal;
layout (location = 2) in vec2 uvs;
// layout(location = 3) in vec3 tangent;
// layout(location = 4) in vec3 bitangent;
// layout(location = 5) in ivec4 boneIds; 
// layout(location = 6) in vec4 weights;

// Uniform variables
uniform mat4 viewProj;
uniform mat4 model;

// Outputs of the vertex shader are the inputs of the same name of the fragment shader.
// The default output, gl_Position, should be assigned something. 
out vec3 fragNormal;
out vec3 fragPos;
out vec2 TexCoords;

void main() {
    // OpenGL maintains the D matrix so you only need to multiply by P, V (aka C inverse), and M
    gl_Position = viewProj * model * vec4(position, 1.0);

    // for shading
	fragNormal = vec3(model * vec4(normal, 0));
	//fragNormal = normal;
    fragPos = vec3(model * vec4(position, 1.0));
    TexCoords = uvs;
}
