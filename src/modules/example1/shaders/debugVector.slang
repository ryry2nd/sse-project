#include "base.slang"

struct VSOutput
{
	float4 position : SV_Position;
	float3 color    : COLOR0;
};

BINDING_CBV(0,0)
cbuffer Camera
{
	mat4 view;
	mat4 proj;
};

// axis line vertices generated from vertex id
VSOutput vertMain(uint vertexID : SV_VertexID)
{
	VSOutput o;

	float3 pos;
	float3 color;

	// X axis (red)
	if (vertexID == 0)
	{
		pos = float3(0,0,0);
		color = float3(1,0,0);
	}
	else if (vertexID == 1)
	{
		pos = float3(1,0,0);
		color = float3(1,0,0);
	}

	// Y axis (green)
	else if (vertexID == 2)
	{
		pos = float3(0,0,0);
		color = float3(0,1,0);
	}
	else if (vertexID == 3)
	{
		pos = float3(0,1,0);
		color = float3(0,1,0);
	}

	// Z axis (blue)
	else if (vertexID == 4)
	{
		pos = float3(0,0,0);
		color = float3(0,0,1);
	}
	else
	{
		pos = float3(0,0,1);
		color = float3(0,0,1);
	}

	o.position = mul(proj, mul(view, float4(pos, 1.0)));
	o.color = color;

	return o;
}

float4 fragMain(VSOutput input) : SV_Target0
{
	return float4(input.color, 1.0);
}