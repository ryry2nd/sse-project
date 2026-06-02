#include ".base.slang"

typedef Input VSInput;
typedef Output VSOutput;

BINDING_IMG(0)
Sampler2D diffuseTexture;

BINDING_CBV(0)
ConstantBuffer<Model> model;

BINDING_CBV(1)
ConstantBuffer<Camera> cam;




[shader("vertex")]
VSOutput vertMain(VSInput input)
{
	mat4 proj = cam.proj;
	mat4 view = cam.view;
	return getPosition(model, view, proj, input);
}



[shader("fragment")]
vec4 fragMain(VSOutput input) : SV_Target
{
	float2 uv = input.TexCoord;

	int2 size;
	diffuseTexture.GetDimensions(size.x, size.y);

	int2 pixel = int2(uv * size);

	float4 texture = diffuseTexture.Load(int3(pixel, 0));

	float edgeFactor = edgeDarken(uv, 0.2f, 0.5f, 0.15f);
	return edgeFactor * texture;
}