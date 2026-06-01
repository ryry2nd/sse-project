#include ".base.slang"

typedef Input VSInput;
typedef Output VSOutput;


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
	mat4 model = models[instanceID];

	return getPosition(model, view, proj, input);
}



[shader("fragment")]
float4 fragMain(VSOutput input) : SV_Target
{
	float edgeFactor = edgeDarken(input.TexCoord, 0.2f, 0.5f, 0.15f);

	float3 green = float3(0.0f, 1.0f, 0.0f);
	float3 inColor = green * edgeFactor;

	float3 fogColor = float3(0.5, 0.6, 0.7);

	return float4(applyFog(inColor, fogColor, input, 0.02), 1.0f);
}