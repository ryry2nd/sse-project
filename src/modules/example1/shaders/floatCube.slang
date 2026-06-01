#include "base.slang"

struct VSInput
{
	float3 aPos      : POSITION;
	float2 aTexCoord : TEXCOORD0;
	float3 aNormal   : NORMAL;
};

struct VSOutput
{
	float2 TexCoord;
	float3 Normal;
	float3 WorldPos;
	float4 position : SV_Position;
};

BINDING_CBV(0,0)
cbuffer Model
{
	mat4 model;
};

BINDING_CBV(1,1)
cbuffer Camera
{
	mat4 view;
	mat4 proj;
};



BINDING_SRV(0,2)
Texture2D diffuseTexture;
BINDING_SAMPLER(0,3)
SamplerState diffuseSampler;

// =======================
// VERTEX
// =======================
[shader("vertex")]
VSOutput vertMain(VSInput input)
{
	VSOutput output;

	// local position
	float4 pos = float4(input.aPos, 1.0f);

	// transforms
	float4 worldPos = mul(model, pos);
	float4 viewPos  = mul(view, worldPos);
	float4 clipPos  = mul(proj, viewPos);

	output.position = clipPos;
	output.WorldPos = worldPos.xyz;

	output.Normal = mul((mat3)model, input.aNormal);
	output.TexCoord = input.aTexCoord;

	return output;
}

// =======================
// FRAGMENT
// =======================
[shader("fragment")]
float4 fragMain(VSOutput input) : SV_Target
{
	float4 texColor = diffuseTexture.Sample(diffuseSampler, input.TexCoord);
	// float2 uv = input.TexCoord - 0.5f;
	// float edge = length(uv);
	// float dark = smoothstep(0.2f, 0.5f, edge);
	// texColor.rgb *= (1.0f - dark * 0.15f);
	return texColor;
}