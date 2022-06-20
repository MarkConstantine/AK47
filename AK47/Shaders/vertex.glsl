#version 330 core

layout(location = 0) in vec3 vertexPosition_modelspace;
layout(location = 1) in vec2 vertexTexCoord_modelspace;
layout(location = 2) in vec3 vertexNormal_modelspace;
layout(location = 3) in vec3 vertexTangent_modelspace;
layout(location = 4) in vec3 vertexBitangent_modelspace;

out vec2 texCoord_modelspace;
out vec3 position_worldspace;
out mat3 TBN;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main()
{
    // Position of vertex in worldspace.
    position_worldspace = (model * vec4(vertexPosition_modelspace, 1)).xyz;

    texCoord_modelspace = vertexTexCoord_modelspace;

    // Tangents and Bitangents in worldspace for normal mapping.
    mat3 normalMatrix = transpose(inverse(mat3(model)));
    vec3 T = normalize(normalMatrix * vertexTangent_modelspace);
    vec3 B = normalize(normalMatrix * vertexBitangent_modelspace);
    vec3 N = normalize(normalMatrix * vertexNormal_modelspace);
    
    // Re-orthogonalize TBN vector so each vector is again perpendicular to the other vectors (Gram-Schmit Process).
    T = normalize(T - dot(T, N) * N);
    
    TBN = transpose(mat3(T, B, N));

    gl_Position = projection * view * model * vec4(vertexPosition_modelspace, 1);
}