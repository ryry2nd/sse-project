#version 330 core
layout(location = 0) in vec3 aPos;
layout(location = 1) in vec2 aTexCoord;
layout(location = 2) in vec3 aNormal;

out vec2 TexCoord;
out vec3 FragPos;
out vec3 Normal;

uniform mat4 uModel;
uniform mat4 uView;
uniform mat4 uProjection;

void main()
{
    vec4 worldPos = uModel * vec4(aPos, 1.0);
    vec4 viewPos = uView * worldPos;

    gl_Position = uProjection *  viewPos;

    FragPos = vec3(worldPos);
    Normal = normalize(mat3(transpose(inverse(uModel))) * aNormal);
    TexCoord = aTexCoord;
}

