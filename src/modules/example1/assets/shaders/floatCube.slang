struct VSInput
{
    float3 aPos      : POSITION;
	float2 aTexCoord : TEXCOORD0;
	float3 aNormal   : NORMAL;
};

struct VSOutput
{
    float2 TexCoord : TEXCOORD0;
    float3 Normal   : TEXCOORD1;
    float3 WorldPos : TEXCOORD2;
    float4 position : SV_Position;
};

cbuffer Model
{
    float4x4 model;
	float3x3 normalMatrix;
};

cbuffer Camera
{
    float4x4 view;
    float4x4 proj;
};

Texture2D diffuseTexture;
SamplerState diffuseSampler;

// =======================
// VERTEX
// =======================
[shader("vertex")]
VSOutput vertMain(VSInput input)
{
    VSOutput output;

    float4 worldPos = mul(model, float4(input.aPos, 1.0));
    output.WorldPos = worldPos.xyz;

    output.position = mul(proj, mul(view, worldPos));

    output.Normal = normalize(mul(normalMatrix, input.aNormal));

    output.TexCoord = input.aTexCoord;

    return output;
}

// =======================
// FRAGMENT
// =======================
[shader("fragment")]
float4 fragMain(VSOutput input) : SV_Target
{
    return diffuseTexture.Sample(diffuseSampler, input.TexCoord);
}