#version 330 core
layout(location = 0) in vec3 aPos;
layout(location = 1) in vec2 aTexCoord;
layout(location = 2) in vec3 aNormal;

out vec2 TexCoord;
out vec3 FragPos;
out vec3 Normal;
flat out int doSimple;

uniform mat4 uModel;
uniform mat4 uView;
uniform mat4 uProjection;

uniform float uScaleFactor;

void main()
{
    vec4 worldPos = uModel * vec4(aPos, 1.0);
    vec4 viewPos = uView * worldPos;

    if (uScaleFactor != 1 && uScaleFactor != 0) {
        //viewPos.z /= uScaleFactor;
        doSimple = 1;
    }
    else {
        doSimple = 0;
    }

    gl_Position = uProjection *  viewPos;

    FragPos = vec3(worldPos);
    Normal = normalize(mat3(transpose(inverse(uModel))) * aNormal);
    TexCoord = aTexCoord;
}

