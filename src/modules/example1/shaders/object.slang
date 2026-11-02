#include ".base.slang"
#include ".colors.slang"

using namespace Structs;
using namespace Normals;
using namespace Utils;
using namespace Fog;

typedef Input VSInput;
typedef Output VSOutput;


BINDING_CBV(0)
ConstantBuffer<Model> model;

BINDING_CBV(1)
ConstantBuffer<Camera> cam;

BINDING_CBV(2)
ConstantBuffer<Material> material;

BINDING_IMG(0)
Sampler2D diffuseTexture;

BINDING_IMG(1)
Sampler2D bumpTexture;

BINDING_IMG(2)
Sampler2D metallicTexture;

BINDING_IMG(3)
Sampler2D roughnessTexture;


[shader("vertex")]
VSOutput vertMain(VSInput input)
{
    return getPosition(model, cam.view, cam.proj, input);
}


[shader("fragment")]
vec4 fragMain(VSOutput input) : SV_Target
{
    vec4 texColor = diffuseTexture.Sample(input.TexCoord);

    vec3 albedo = texColor.rgb;

    vec3 N = GetNormalFromMap(
        input.Normal,
        input.ViewPos.xyz,
        input.TexCoord,
		bumpTexture
    );

    vec3 L = normalize(vec3(0.4, 0.8, 0.5));

    float NdotL = max(dot(N, L), 0.0);

    // Basic diffuse lighting.
    vec3 diffuse = albedo * (0.35 + 0.65 * NdotL);

    // Metallic and roughness only affect the highlight.
    float metallic = metallicTexture.Sample(input.TexCoord).r;
    float roughness = roughnessTexture.Sample(input.TexCoord).r;

    vec3 V = normalize(-input.ViewPos.xyz);
    vec3 H = normalize(L + V);

    float shininess = lerp(128.0, 4.0, roughness);
    float highlight = pow(max(dot(N, H), 0.0), shininess);

    vec3 specularColor = lerp(
        vec3(0.04),
        albedo,
        metallic
    );

    vec3 color = diffuse + specularColor * highlight;

    color += material.emissive;

    return vec4(color, texColor.a * material.opacity);
}