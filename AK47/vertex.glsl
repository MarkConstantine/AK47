#version 330 core

// Input vertex data, different for all executions of this shader.
layout(location = 0) in vec3 vertexPosition_modelspace;
layout(location = 1) in vec2 vertexUV;
layout(location = 2) in vec3 vertexNormal_modelspace;

out vec2 uv;
out vec3 position_worldspace;
out vec3 eyeDirection_cameraspace;
out vec3 lightDirection_cameraspace;
out vec3 normal_cameraspace;

// Values that stay constant for whole mesh
uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform vec3 lightPosition_worldspace;

void main()
{
    mat4 mvp = projection * view * model;
    
    gl_Position = mvp * vec4(vertexPosition_modelspace, 1);

    // Position of vertex in worldspace.
    position_worldspace = (model * vec4(vertexPosition_modelspace, 1)).xyz;

    // Vector from vertex to camera.
    vec3 vertexPosition_cameraspace = (view * model * vec4(vertexPosition_modelspace, 1)).xyz;
    eyeDirection_cameraspace = vec3(0, 0, 0) - vertexPosition_cameraspace;

    // Vector from vertex to light. M is ommited because it's identity in cameraspace.
    vec3 lightPosition_cameraspace = (view * vec4(lightPosition_worldspace, 1)).xyz;
    lightDirection_cameraspace = lightPosition_cameraspace + eyeDirection_cameraspace;

    normal_cameraspace = (view * model * vec4(vertexNormal_modelspace, 0)).xyz; // Only correct if ModelMatrix does not scale the model! Use its inverse transpose if not.

    uv = vertexUV;
}