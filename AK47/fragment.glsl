#version 330 core

in vec2 uv;
in vec3 position_worldspace;
in vec3 eyeDirection_cameraspace;
in vec3 lightDirection_cameraspace;
in vec3 normal_cameraspace;

out vec4 color;

uniform sampler2D textureSampler;
uniform vec3 lightPosition_worldspace;
uniform vec3 lightColor;
uniform float lightPower;

void main()
{
    // Material properties
    vec3 diffuseColor = texture(textureSampler, uv).rgb;
    vec3 ambientColor = vec3(0.3, 0.3, 0.3) * diffuseColor;
    vec3 specularColor = vec3(0.3, 0.3, 0.3);

    // Distance to light
    float distance = length(lightPosition_worldspace - position_worldspace);

    vec3 n = normalize(normal_cameraspace);
    vec3 l = normalize(lightDirection_cameraspace);
    float cosTheta = clamp(dot(n, l), 0, 1); // 0 = perpendicular or behind triangle, 1 = vertical

    // Eye vector
    vec3 e = normalize(eyeDirection_cameraspace);
    vec3 r = reflect(-l, n);
    float cosAlpha = clamp(dot(e, r), 0, 1); // 1 = looking into reflection, <1 = looking elsewhere

    vec3 colorRGB = 
        // Ambient: simulates indirect light
        ambientColor + 
        // Diffuse: color of the object.
        diffuseColor * lightColor * lightPower * cosTheta / (distance * distance) +
        // Specular: reflective highlight
        specularColor * lightColor * lightPower * pow(cosAlpha, 5) / (distance * distance);

    color = vec4(colorRGB.x, colorRGB.y, colorRGB.z, 0.7);
}