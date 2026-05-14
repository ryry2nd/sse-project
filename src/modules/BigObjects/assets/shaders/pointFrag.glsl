#version 330 core
out vec4 FragColor;
uniform vec3 color;
uniform float depth;

void main() {
    float dist = length((gl_PointCoord - vec2(0.5)) * 2.0);

    if (dist > 1.0)
        discard;

    float alpha = smoothstep(1.0, 0.6, dist);

    FragColor = vec4(color, alpha); // white, soft-edged

    gl_FragDepth = depth;
}
