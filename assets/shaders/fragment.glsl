#version 330 core

in vec2 TexCoord;
in vec3 FragPos;
in vec3 Normal;

out vec4 FragColor;

uniform sampler2D texture1;

uniform float gamma;
uniform bool u_fullBright;
uniform float depth;

void main()
{
    vec3 texColor = texture(texture1, TexCoord).rgb;
    vec3 finalColor = vec3(0.0);
    float ambientStrength = 0.1;
    float specularStrength = 0.5;

    finalColor = texColor;
    // if (u_fullBright) {
    //     finalColor = texColor + emissionColor;
//     }
//     else {
//         vec3 norm = normalize(Normal);
//         vec3 lighting = vec3(0.0);
//         vec3 viewDir = normalize(-FragPos);
//         vec3 lightDir;
//         vec3 ambient;
//         float diff;
//         vec3 diffuse;
//         vec3 reflectDir;
//         float spec;
//         vec3 specular;
//         // Directional Light
//         for (int i = 0; i < numLights; i++)
//         {
//             lightDir = normalize(-lightPositions[i]);
// 
//             ambient = ambientStrength * lightColors[i];
// 
//             diff = max(dot(norm, lightDir), 0.0);
//             diffuse = diff * lightColors[i];
// 
//             reflectDir = reflect(-lightDir, norm);
//             spec = pow(max(dot(viewDir, reflectDir), 0.0), 32);
//             specular = specularStrength * spec * lightColors[i];
// 
//             lighting += ambient + diffuse + specular;
//         }
//         finalColor = lighting * texColor + emissionColor;
//     }
    FragColor = vec4(pow(finalColor, vec3(1.0 / gamma)), 1.0);

    gl_FragDepth = depth;
}
