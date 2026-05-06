struct VSInput
{
	float3 aPos      : POSITION;
	float2 aTexCoord : TEXCOORD0;
};

struct VSOutput
{
	float2 TexCoord : TEXCOORD0;
	float4 position : SV_Position;
};

struct PSInput
{
	float2 TexCoord : TEXCOORD0;
};

cbuffer Camera
{
	float4x4 view;
	float4x4 proj;
};

StructuredBuffer<float4x4> models;

Texture2D<float4> im1;
SamplerState im1_sampler;

// =======================
// VERTEX
// =======================
[shader("vertex")]
VSOutput vertMain(VSInput input, uint instanceID : SV_InstanceID)
{
	VSOutput output;

	float4x4 model = models[instanceID];

	output.TexCoord = input.aTexCoord;

	float4 worldPos = mul(model, float4(input.aPos, 1.0));
	output.position = mul(proj, mul(view, worldPos));

	return output;
}

// =======================
// FRAGMENT
// =======================
[shader("fragment")]
float4 fragMain(VSOutput input) : SV_Target
{
	return im1.Sample(im1_sampler, input.TexCoord);
}