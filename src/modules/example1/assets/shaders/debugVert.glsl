#version 460 core

layout(location = 0) in vec3 aPos;
layout(location = 1) in vec2 aTexCoord;

out vec2 TexCoord;

layout(std140) uniform Camera
{
    mat4 view;
    mat4 proj;
};
layout(std140) uniform pos
{
    mat4 model;
};

void main()
{
    TexCoord = aTexCoord;
    gl_Position = proj * view * model * vec4(aPos, 1.0);
}