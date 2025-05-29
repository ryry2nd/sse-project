#version 330 core
out vec4 FragColor;
uniform vec3 color;
uniform float gamma;
uniform float depth;

void main() {
    vec2 coord = gl_PointCoord - vec2(0.5);
    float dist = length(coord * 2.0);

    if (dist > 1.0)
        discard;

    float alpha = smoothstep(1.0, 0.6, dist);

    vec3 correctedColor = pow(color, vec3(1.0 / gamma));

    FragColor = vec4(correctedColor, alpha); // white, soft-edged

    gl_FragDepth = depth;
}
