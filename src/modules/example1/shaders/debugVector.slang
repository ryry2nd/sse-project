#include ".base.slang"

struct VSOutput
{
	vec4 position : SV_Position;
	vec3 color    : COLOR0;
};

BINDING_CBV(0)
ConstantBuffer<Camera> cam;

// axis line vertices generated from vertex id
VSOutput vertMain(uint vertexID : SV_VertexID)
{
	mat4 proj = cam.proj;
	mat4 view = cam.view;

	VSOutput o;

	vec3 pos;
	vec3 color;

	// X axis (red)
	if (vertexID == 0)
	{
		pos = vec3(0,0,0);
		color = vec3(1,0,0);
	}
	else if (vertexID == 1)
	{
		pos = vec3(1,0,0);
		color = vec3(1,0,0);
	}

	// Y axis (green)
	else if (vertexID == 2)
	{
		pos = vec3(0,0,0);
		color = vec3(0,1,0);
	}
	else if (vertexID == 3)
	{
		pos = vec3(0,1,0);
		color = vec3(0,1,0);
	}

	// Z axis (blue)
	else if (vertexID == 4)
	{
		pos = vec3(0,0,0);
		color = vec3(0,0,1);
	}
	else
	{
		pos = vec3(0,0,1);
		color = vec3(0,0,1);
	}

	o.position = mul(proj, mul(view, vec4(pos, 1.0)));
	o.color = color;

	return o;
}

vec4 fragMain(VSOutput input) : SV_Target0
{
	return vec4(input.color, 1.0);
}