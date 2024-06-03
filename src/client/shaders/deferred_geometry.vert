#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoords;
layout (location = 3) in ivec4 boneIds; 
layout (location = 4) in vec4 weights;

const int MAX_BONES = 100;
const int MAX_BONE_INFLUENCE = 4;

out vec3 FragPos;
out vec2 TexCoords;
out vec3 FragNormal;

uniform mat4 model;
uniform mat4 viewProj;
uniform mat4 finalBonesMatrices[MAX_BONES];

void main()
{
    int skipped = 0;
    vec4 totalPosition = vec4(0);
    vec4 totalNormal = vec4(0);
    
    for(int i = 0 ; i < MAX_BONE_INFLUENCE ; i++) {
        if(boneIds[i] <= -1) {
            skipped += 1;
            continue;
        }

        if(boneIds[i] >= MAX_BONES) {
            totalPosition = vec4(aPos, 1);
            totalNormal = vec4(aNormal, 0);
            break;
        }
        
        vec4 localPosition = finalBonesMatrices[boneIds[i]] * vec4(aPos, 1);
        totalPosition += localPosition * weights[i];
        vec4 localNormal = finalBonesMatrices[boneIds[i]] * vec4(aNormal, 0);
        totalNormal += localNormal * weights[i];
    }

    if (skipped == MAX_BONE_INFLUENCE) {
        totalPosition = vec4(aPos, 1);
        totalNormal = vec4(aNormal, 0);
    }

    
    vec4 worldPos = model * totalPosition;
    FragPos = worldPos.xyz; 
    TexCoords = aTexCoords;
    
    mat3 normalMatrix = transpose(inverse(mat3(model)));
    FragNormal = normalMatrix * vec3(totalNormal);

    gl_Position = viewProj * worldPos;
    
    /*
    vec4 worldPos = model * vec4(aPos, 1.0);
    FragPos = worldPos.xyz; 
    TexCoords = aTexCoords;
    
    mat3 normalMatrix = transpose(inverse(mat3(model)));
    FragNormal = normalMatrix * aNormal;

    gl_Position = viewProj * worldPos;
    */
}


