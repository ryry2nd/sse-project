#include ".colors.slang"
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


	float3 color = COLOR_GREEN * edgeFactor;
	float3 fogColor = COLOR_GRAY_FOG;

	return float4(applyFog(color, fogColor, input, 0.02), 1.0f);
}