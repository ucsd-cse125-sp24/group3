#version 330 core

#define NR_POINT_LIGHTS 32

layout (location = 0) out vec3 gPosition;
layout (location = 1) out vec3 gNormal;
layout (location = 2) out vec4 gAlbedoSpec;
// layout (location = 3) out vec3 gTangentNormal;

in vec3 FragPos;
in vec3 FragNormal;
in vec2 TexCoords;
in mat3 TBN;

uniform sampler2D texture_diffuse1;
uniform sampler2D texture_specular1;
uniform sampler2D texture_normal1;
uniform bool has_normal_map;

void main()
{    
    // store the fragment position vector in the first gbuffer texture
    gPosition = FragPos;
    // also store the per-fragment normals into the gbuffer
    if (has_normal_map) {
        // obtain normal from normal map in range [0,1]
        gNormal = texture(texture_normal1, TexCoords).rgb;
        // transform normal vector to range [-1,1]
        gNormal = gNormal * 2.0 - 1.0;
        gNormal = normalize(TBN * gNormal);
        gNormal = normalize(gNormal);
        // if (gNormal == vec3(0.0, 0.0, 0.0)) {
        //     gAlbedoSpec.rgb = vec3(1.0, 0.0, 0.0);
        // } else {
        //     gAlbedoSpec.rgb = vec3(0.0, 1.0, 0.0);
        // }

        // gNormal = normalize(TBN * (texture(texture_normal1, TexCoords).rgb * 2.0 - 1.0));
        // gNormal = vec3(0.0, 1.0, 0.0);
        // gAlbedoSpec.rgb = texture(texture_normal1, TexCoords).rgb;
    } else {
        gNormal = normalize(FragNormal);
        // gAlbedoSpec.rgb = texture(texture_diffuse1, TexCoords).rgb;
    }

    // and the diffuse per-fragment color
    gAlbedoSpec.rgb = texture(texture_diffuse1, TexCoords).rgb;
    // store specular intensity in gAlbedoSpec's alpha component
    gAlbedoSpec.a = texture(texture_specular1, TexCoords).r;

    // gTangentNormal = texture(texture_normal, TexCoords).rgb;
}
