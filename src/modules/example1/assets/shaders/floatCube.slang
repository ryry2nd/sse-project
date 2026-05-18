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



// Texture2D diffuseTexture;
// SamplerState diffuseSampler;

// =======================
// VERTEX
// =======================
[shader("vertex")]
VSOutput vertMain(VSInput input)
{
	float4x4 identity =
	{
		1.0, 0.0, 0.0, 0.0,
		0.0, 1.0, 0.0, 0.0,
		0.0, 0.0, 1.0, 0.0,
		0.0, 0.0, 0.0, 1.0
	};

	// float4x4 model = identity;
	// float4x4 view = identity;
	// float4x4 proj = identity;

	VSOutput output;

    // local position
    float4 pos = float4(input.aPos, 1.0f);

    // move cube forward into visible space
    pos.z -= 3.0f;

    // transforms
    float4 worldPos = mul(model, pos);
    float4 viewPos  = mul(view, worldPos);
    float4 clipPos  = mul(proj, viewPos);

    // outputs
    output.position = clipPos;
    output.WorldPos = worldPos.xyz;

    output.Normal = normalize(mul(input.aNormal, normalMatrix));

    output.TexCoord = input.aTexCoord;

    return output;
}

// =======================
// FRAGMENT
// =======================
[shader("fragment")]
float4 fragMain(VSOutput input) : SV_Target
{
    // return diffuseTexture.Sample(diffuseSampler, input.TexCoord);
	return float4(0.0f, 1.0f, 0.0f, 1.0f);
}