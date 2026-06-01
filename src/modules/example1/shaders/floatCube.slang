#include ".base.slang"

typedef Input VSInput;
typedef Output VSOutput;

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
	return getPosition(model, view, proj, input);
}

// =======================
// FRAGMENT
// =======================
[shader("fragment")]
float4 fragMain(VSOutput input) : SV_Target
{
	float4 texColor = diffuseTexture.Sample(diffuseSampler, input.TexCoord);
	return texColor;
}