typedef float4x4 mat4;
typedef float3x3 mat3;

#if defined(VULKAN)
	#define BINDING_SRV(x, y)     [[vk::binding(x, y)]]
	#define BINDING_UAV(x, y)     [[vk::binding(x, y)]]
	#define BINDING_CBV(x, y)     [[vk::binding(x, y)]]
	#define BINDING_SAMPLER(x, y) [[vk::binding(x, y)]]
#elif defined(DX12)
	#define BINDING_SRV(x, y)     [[register(t##x, space##y)]]
	#define BINDING_UAV(x, y)     [[register(u##x, space##y)]]
	#define BINDING_CBV(x, y)     [[register(b##x, space##y)]]
	#define BINDING_SAMPLER(x, y) [[register(s##x, space##y)]]
#else
	// Read-only textures/buffers
	#define BINDING_SRV(x, y)
	// Read-write textures/buffers
	#define BINDING_UAV(x, y)
	// Constant buffers
	#define BINDING_CBV(x, y)
	// Samplers
	#define BINDING_SAMPLER(x, y)
#endif

float3 transformNormal(float3 normal, mat4 model)
{
    return normalize(mul((mat3)model, normal));
}

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