#include ".base.slang"

typedef Input VSInput;
typedef Output VSOutput;

BINDING_CBV(0,0)
ConstantBuffer<Model> model;

BINDING_CBV(1,1)
ConstantBuffer<Camera> cam;



BINDING_SRV(0,2)
Texture2D diffuseTexture;
BINDING_SAMPLER(0,3)
SamplerState diffuseSampler;



[shader("vertex")]
VSOutput vertMain(VSInput input)
{
	mat4 proj = cam.proj;
	mat4 view = cam.view;
	return getPosition(model, view, proj, input);
}



[shader("fragment")]
float4 fragMain(VSOutput input) : SV_Target
{
	float4 texColor = diffuseTexture.Sample(diffuseSampler, input.TexCoord);
	return texColor;
}