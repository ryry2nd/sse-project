typedef float4x4 mat4;
typedef float3x3 mat3;

#if defined(VULKAN)
	#define BINDING_SRV(x)     [[vk::binding(x, 0)]]
	#define BINDING_UAV(x)     [[vk::binding(x, 0)]]
	#define BINDING_CBV(x)     [[vk::binding(x, 0)]]
	#define BINDING_SAMPLER(x) [[vk::binding(x, 0)]]
#elif defined(DX12)
	#define BINDING_SRV(x)     [[register(t##x, space0)]]
	#define BINDING_UAV(x)     [[register(u##x, space0)]]
	#define BINDING_CBV(x)     [[register(b##x, space0)]]
	#define BINDING_SAMPLER(x) [[register(s##x, space0)]]
#else
	// Read-only textures/buffers
	#define BINDING_SRV(x)
	// Read-write textures/buffers
	#define BINDING_UAV(x)
	// Constant buffers
	#define BINDING_CBV(x)
	// Samplers
	#define BINDING_SAMPLER(x)
#endif


struct Input
{
	float3 aPos      : POSITION;
	float2 aTexCoord : TEXCOORD0;
	float3 aNormal   : NORMAL;
};

struct Output
{
	float2 TexCoord;
	float3 Normal;
	float3 WorldPos;
	float4 ViewPos;
	float4 position : SV_Position;
};

struct Camera
{
	mat4 view;
	mat4 proj;
};

typedef mat4 Model;

float3 transformNormal(float3 normal, mat4 model)
{
    return normalize(mul((mat3)model, normal));
}

float edgeDarken(float2 texCoord, float inner, float outer, float strength)
{
	float2 uv = texCoord - 0.5f;
    float edge = length(uv);
    float dark = smoothstep(inner, outer, edge);
    return 1.0 - dark * strength;
}


float fogExp2(float dist, float density)
{
    return exp(-(dist * density) * (dist * density));
}


float3 applyFog(float3 inColor, float3 fogColor, Output input, float strength) {
	float dist = abs(input.ViewPos.z);
	float fog = fogExp2(dist, strength);
	return lerp(fogColor, inColor, fog);
}

Output getPosition(mat4 model, mat4 view, mat4 proj, Input input) {
	Output output;

	float4 worldPos = mul(model, float4(input.aPos, 1.0));

	output.WorldPos = worldPos.xyz;

	output.Normal = transformNormal(input.aNormal, model);

	output.TexCoord = input.aTexCoord;

	output.ViewPos = mul(view, worldPos);

	output.position = mul(proj, output.ViewPos);

	return output;
}