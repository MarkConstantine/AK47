#version 330 core

layout(location = 0) in vec3 vertexPosition_modelspace;
layout(location = 1) in vec2 vertexTexCoord_modelspace;
layout(location = 2) in vec3 vertexNormal_modelspace;
layout(location = 3) in vec3 vertexTangent_modelspace;
layout(location = 4) in vec3 vertexBitangent_modelspace;

out VS_OUT {
    vec2 texCoord_modelspace;
    vec3 position_worldspace;
    mat3 TBN;
} vs_out;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main()
{
    // Tangents and Bitangents in worldspace for normal mapping.
    mat3 normalMatrix = transpose(inverse(mat3(model)));
    vec3 T = normalize(normalMatrix * vertexTangent_modelspace);
    vec3 B = normalize(normalMatrix * vertexBitangent_modelspace);
    vec3 N = normalize(normalMatrix * vertexNormal_modelspace);
    
    // Re-orthogonalize TBN vector so each vector is again perpendicular to the other vectors (Gram-Schmit Process).
    T = normalize(T - dot(T, N) * N);
    
    vs_out.TBN = transpose(mat3(T, B, N));
    vs_out.position_worldspace = (model * vec4(vertexPosition_modelspace, 1.0)).xyz;
    vs_out.texCoord_modelspace = vertexTexCoord_modelspace;

    gl_Position = projection * view * model * vec4(vertexPosition_modelspace, 1.0);
}