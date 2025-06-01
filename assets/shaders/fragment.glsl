#version 330 core

in vec2 TexCoord;
in vec3 FragPos;
in vec3 Normal;
flat in int doSimple;

out vec4 FragColor;

uniform float u_CullRadius;
uniform sampler2D texture1;
uniform vec3 emissionColor;

const int MAX_LIGHTS = 127;
uniform int numLights;
uniform vec3 lightPositions[MAX_LIGHTS];
uniform vec3 lightColors[MAX_LIGHTS];

uniform float gamma;
uniform bool u_fullBright;
uniform float depth;

void main()
{
    if (doSimple == 0 && dot(FragPos, FragPos) < u_CullRadius) discard;

    vec3 texColor = texture(texture1, TexCoord).rgb;
    vec3 finalColor = vec3(0.0);
    float ambientStrength = 0.1;
    float specularStrength = 0.5;

    if (u_fullBright) {
        finalColor = texColor + emissionColor;
    }
    else if (doSimple == 1) {
        vec3 lighting = vec3(0.0);
        vec3 norm = Normal;
        vec3 lightDir;
        vec3 ambient;
        float diff;
        vec3 diffuse;
        // Directional Light
        for (int i = 0; i < numLights; i++)
        {
            lightDir = -lightPositions[i];

            ambient = ambientStrength * lightColors[i];

            diff = max(dot(norm, lightDir), 0.0);
            diffuse = diff * lightColors[i];

            lighting += ambient + diffuse;
        }
        finalColor = lighting * texColor + emissionColor;
    }
    else {
        vec3 norm = normalize(Normal);
        vec3 lighting = vec3(0.0);
        vec3 viewDir = normalize(-FragPos);
        vec3 lightDir;
        vec3 ambient;
        float diff;
        vec3 diffuse;
        vec3 reflectDir;
        float spec;
        vec3 specular;
        // Directional Light
        for (int i = 0; i < numLights; i++)
        {
            lightDir = normalize(-lightPositions[i]);

            ambient = ambientStrength * lightColors[i];

            diff = max(dot(norm, lightDir), 0.0);
            diffuse = diff * lightColors[i];

            reflectDir = reflect(-lightDir, norm);
            spec = pow(max(dot(viewDir, reflectDir), 0.0), 32);
            specular = specularStrength * spec * lightColors[i];

            lighting += ambient + diffuse + specular;
        }
        finalColor = lighting * texColor + emissionColor;
    }
    FragColor = vec4(pow(finalColor, vec3(1.0 / gamma)), 1.0);

    gl_FragDepth = depth;
}
