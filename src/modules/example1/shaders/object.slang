#include ".base.slang"
#include ".colors.slang"

typedef Input VSInput;
typedef Output VSOutput;

BINDING_CBV(0)
ConstantBuffer<Model> model;

BINDING_CBV(1)
ConstantBuffer<Camera> cam;

BINDING_CBV(2)
ConstantBuffer<Material> material;


[shader("vertex")]
VSOutput vertMain(VSInput input)
{
    return getPosition(model, cam.view, cam.proj, input);
}


[shader("fragment")]
vec4 fragMain(VSOutput input) : SV_Target
{
    vec3 normal = normalize(input.Normal);

    // Material colors
    vec3 color = material.diffuse;

    // Ambient
    color += material.ambient * 0.2;

    // Emission
    color += material.emissive;

    // Simple lighting
    vec3 lightDir = normalize(vec3(0.5, 1.0, 0.5));
    vec3 viewDir = normalize(-input.ViewPos.xyz);
    vec3 halfwayDir = normalize(lightDir + viewDir);

    // Diffuse lighting
    float diff = max(dot(normal, lightDir), 0.0);

    // Specular lighting
    float spec = pow(
        max(dot(normal, halfwayDir), 0.0),
        max(material.shininess, 1.0)
    );

    color *= (0.2 + diff);
    color += material.specular * spec;

    return vec4(color, material.opacity);
}