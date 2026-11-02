#include ".base.slang"
#include ".colors.slang"

typedef Input VSInput;
typedef Output VSOutput;

static const float PI = 3.14159265359;

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


// --------------------------------------------------
// PBR FUNCTIONS
// --------------------------------------------------

float DistributionGGX(vec3 N, vec3 H, float roughness)
{
    float a = roughness * roughness;
    float a2 = a * a;

    float NdotH = max(dot(N, H), 0.0);
    float NdotH2 = NdotH * NdotH;

    float denom = NdotH2 * (a2 - 1.0) + 1.0;

    return a2 / max(PI * denom * denom, 0.0001);
}

float GeometrySchlickGGX(float NdotV, float roughness)
{
    float r = roughness + 1.0;
    float k = (r * r) / 8.0;

    return NdotV / max(NdotV * (1.0 - k) + k, 0.0001);
}

float GeometrySmith(vec3 N, vec3 V, vec3 L, float roughness)
{
    float NdotV = max(dot(N, V), 0.0);
    float NdotL = max(dot(N, L), 0.0);

    return GeometrySchlickGGX(NdotV, roughness) *
           GeometrySchlickGGX(NdotL, roughness);
}

vec3 FresnelSchlick(float cosTheta, vec3 F0)
{
    float f = pow(clamp(1.0 - cosTheta, 0.0, 1.0), 5.0);

    return F0 + (vec3(1.0) - F0) * f;
}


// --------------------------------------------------
// NORMAL MAPPING
// --------------------------------------------------

vec3 SafeNormalize(vec3 v, vec3 fallback)
{
    float lenSq = dot(v, v);

    if (lenSq < 0.000001)
        return fallback;

    return v * rsqrt(lenSq);
}

vec3 GetNormalFromMap(vec3 normal, vec3 position, vec2 uv)
{
    vec3 N = SafeNormalize(normal, vec3(0.0, 0.0, 1.0));

    vec3 dp1 = ddx(position);
    vec3 dp2 = ddy(position);

    vec2 duv1 = ddx(uv);
    vec2 duv2 = ddy(uv);

    vec3 T = dp1 * duv2.y - dp2 * duv1.y;
    vec3 B = -dp1 * duv2.x + dp2 * duv1.x;

    T = T - N * dot(N, T);
    T = SafeNormalize(T, vec3(1.0, 0.0, 0.0));

    // Preserve UV orientation.
    float handedness = dot(cross(N, T), B) < 0.0 ? -1.0 : 1.0;
    B = cross(N, T) * handedness;

    vec3 normalMap = bumpTexture.Sample(uv).rgb;
    normalMap = normalMap * 2.0 - 1.0;

    return SafeNormalize(
        T * normalMap.x +
        B * normalMap.y +
        N * normalMap.z,
        N
    );
}


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
        input.TexCoord
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