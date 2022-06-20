#version 330 core

in VS_OUT {
    vec2 texCoord_modelspace;
    vec3 position_worldspace;
    mat3 TBN;
} fs_in;

out vec4 fragColor;

uniform sampler2D diffuseMap;
uniform sampler2D normalMap;
uniform sampler2D specularMap;

uniform vec3 cameraPosition_worldspace;
uniform vec3 lightPosition_worldspace;

void main()
{
    vec3 diffuse = texture(diffuseMap, fs_in.texCoord_modelspace).rgb;
    vec3 normal = texture(normalMap, fs_in.texCoord_modelspace).rgb * 2.0 - 1.0; // Map [0..1] to [-1,1]
    vec3 specular = vec3(1.0) * texture(specularMap, fs_in.texCoord_modelspace).r; // Red channel only used. Use same amount for green and blue channel.

    vec3 lightDirection_worldspace = lightPosition_worldspace - fs_in.position_worldspace;
    vec3 lightDirection_tangentspace = normalize(fs_in.TBN * lightDirection_worldspace);
    vec3 viewDirection_worldspace = cameraPosition_worldspace - fs_in.position_worldspace;
    vec3 viewDirection_tangentspace = normalize(fs_in.TBN * viewDirection_worldspace);
    vec3 reflectDirection_tangentspace = reflect(-lightDirection_tangentspace, normal);
    vec3 halfwayDirection_tangentspace = normalize(lightDirection_tangentspace + viewDirection_tangentspace);
    
    // Ambient Color: Simulate indirect light
    vec3 ambientColor = 0.1 * diffuse;

    // Diffuse Color: Color of the object.
    vec3 diffuseColor = vec3(1.0, 1.0, 1.0) * diffuse * max(dot(lightDirection_tangentspace, normal), 0.0);

    // Specular Color: Reflective highlights.
    vec3 specularColor = specular * vec3(0.2) * pow(max(dot(normal, halfwayDirection_tangentspace), 0.0), 32);
    
    fragColor = vec4(ambientColor + diffuseColor + specularColor, 1.0);
}