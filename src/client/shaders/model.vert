#version 330 core
#
// Vertex shader for loaded models.
// Also forwards texture coordinates to fragment shader.

layout (location = 0) in vec3 position;
layout (location = 1) in vec3 normal;
layout (location = 2) in vec2 uvs;
layout (location = 3) in vec3 tangent;
layout (location = 4) in vec3 bitangent;
layout (location = 5) in ivec4 boneIds; 
layout (location = 6) in vec4 weights;

const int MAX_BONES = 100;
const int MAX_BONE_INFLUENCE = 4;

// Uniform variables
uniform mat4 viewProj;
uniform mat4 model;
uniform mat4 finalBonesMatrices[MAX_BONES];

// Outputs of the vertex shader are the inputs of the same name of the fragment shader.
// The default output, gl_Position, should be assigned something. 
out vec3 fragNormal;
out vec3 fragPos;
out vec2 TexCoords;

void main() {
    vec4 totalPosition = vec4(0);
    vec4 totalNormal = vec4(0);
    
    for(int i = 0 ; i < MAX_BONE_INFLUENCE ; i++) {
        if(boneIds[i] <= -1) {
            continue;
        }

        if(boneIds[i] >= MAX_BONES) {
            totalPosition = vec4(position, 1);
            totalNormal = vec4(normal, 0);
            break;
        }
        
        vec4 localPosition = finalBonesMatrices[boneIds[i]] * vec4(position, 1);
        totalPosition += localPosition * weights[i];
        vec4 localNormal = finalBonesMatrices[boneIds[i]] * vec4(normal, 0);
        totalNormal += localNormal * weights[i];
    }
   
    // OpenGL maintains the D matrix so you only need to multiply by P, V (aka C inverse), and M
    // for shading

    gl_Position = viewProj * model * totalPosition;
	fragNormal = vec3(model * totalNormal);
    fragPos = vec3(model * totalPosition);

    /*
    gl_Position = viewProj * model * vec4(position, 1);
	fragNormal = vec3(model * vec4(normal, 0));
    fragPos = vec3(model * vec4(position, 1));
    */

    TexCoords = uvs;
}
