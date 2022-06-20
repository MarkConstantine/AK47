#version 330 core

in vec2 texCoord_modelspace;
in vec3 position_worldspace;
in mat3 TBN;

out vec4 fragColor;

uniform sampler2D diffuseMap;
uniform sampler2D normalMap;
uniform vec3 cameraPosition_worldspace;
uniform vec3 lightPosition_worldspace;
uniform vec3 lightColor;
uniform float lightPower;

void main()
{
    vec3 normal = texture(normalMap, texCoord_modelspace).rgb * 2.0 - 1.0; // Map [0..1] to [-1,1]
    vec3 color = texture(diffuseMap, texCoord_modelspace).rgb;

    vec3 lightDirection_worldspace = lightPosition_worldspace - position_worldspace;
    vec3 lightDirection_tangentspace = normalize(TBN * lightDirection_worldspace);
    vec3 viewDirection_worldspace = cameraPosition_worldspace - position_worldspace;
    vec3 viewDirection_tangentspace = normalize(TBN * viewDirection_worldspace);
    vec3 reflectDirection_tangentspace = reflect(-lightDirection_tangentspace, normal);
    vec3 halfwayDirection_tangentspace = normalize(lightDirection_tangentspace + viewDirection_tangentspace);
    
    // Ambient Color: Simulate indirect light
    vec3 ambientColor = 0.1 * color;

    // Diffuse Color: Color of the object.
    vec3 diffuseColor = color * max(dot(lightDirection_tangentspace, normal), 0);

    // Specular Color: Reflective highlights.
    vec3 specularColor = vec3(0.2) * pow(max(dot(normal, halfwayDirection_tangentspace), 0), 32);
    
    fragColor = vec4(ambientColor + diffuseColor + specularColor, 1.0);
}