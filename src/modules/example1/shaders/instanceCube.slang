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
cbuffer Camera
{
	mat4 view;
	mat4 proj;
};

BINDING_SRV(0,1)
StructuredBuffer<mat4> models;

// =======================
// VERTEX
// =======================
[shader("vertex")]
VSOutput vertMain(VSInput input, uint instanceID : SV_InstanceID)
{
	VSOutput output;

	mat4 model = models[instanceID];

	float4 worldPos = mul(model, float4(input.aPos, 1.0));

	output.WorldPos = worldPos.xyz;

	output.Normal = mul((mat3)model, input.aNormal);

	output.TexCoord = input.aTexCoord;

	output.position = mul(proj, mul(view, worldPos));

	return output;
}

[shader("fragment")]
float4 fragMain(VSOutput input) : SV_Target
{
	float2 uv = input.TexCoord - 0.5f;
	float edge = length(uv);

	float dark = smoothstep(0.2f, 0.5f, edge);

	float3 green = float3(0.0f, 1.0f, 0.0f);
	float3 color = green * (1.0f - dark * 0.15f);

	return float4(color, 1.0f);
}