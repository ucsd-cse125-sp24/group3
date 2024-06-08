#version 330 core

#define NR_POINT_LIGHTS 32

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoords;
layout (location = 3) in ivec4 boneIds; 
layout (location = 4) in vec4 weights;
layout (location = 5) in vec3 aTangent;
layout (location = 6) in vec3 aBitangent;

const int MAX_BONES = 100;
const int MAX_BONE_INFLUENCE = 4;

out vec3 FragPos;
out vec3 FragNormal;
out vec2 TexCoords;
out mat3 TBN;

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

    vec3 normal0 = (model * vec4(aNormal, 1.0)).xyz;
    vec3 tangent0 = (model * vec4(aTangent, 1.0)).xyz;

    vec3 Normal = normalize(normal0);
    vec3 Tangent = normalize(tangent0);
    Tangent = normalize(Tangent - dot(Tangent, Normal) * Normal);
    vec3 Bitangent = cross(Normal, Tangent);
    TBN = mat3(Tangent, Bitangent, Normal);

    mat3 normalMatrix = transpose(inverse(mat3(model)));
    // vec3 T = normalize(normalMatrix * aTangent);
    // vec3 B = normalize(normalMatrix * aBitangent);
    // vec3 N = normalize(normalMatrix * aNormal);
    // // T = normalize(T - dot(T, N) * N);
    // // vec3 B = cross(N, T);
    // 
    // TBN = mat3(T, B, N);

    FragNormal = normalMatrix * aNormal;

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


