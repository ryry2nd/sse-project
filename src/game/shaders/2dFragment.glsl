#version 330 core

in vec2 TexCoord;

uniform sampler2D texture1;

out vec4 FragColor;

void main()
{
    vec4 sampled = texture(texture1, TexCoord);
    FragColor = vec4(sampled.rgb, sampled.a);
    gl_FragDepth = 0.0f;
}