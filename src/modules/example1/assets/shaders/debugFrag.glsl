#version 460 core

in vec2 TexCoord;
out vec4 FragColor;

uniform sampler2D im1;

void main()
{
    FragColor = texture(im1, TexCoord);
}