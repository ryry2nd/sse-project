#include ".base.slang"

typedef Input VSInput;
typedef Output VSOutput;

BINDING_IMG(0)
Sampler2D diffuseTexture;

BINDING_CBV(0)
ConstantBuffer<mat4> proj;


[shader("vertex")]
VSOutput vertMain(VSInput input, uint vid : SV_VertexID)
{
	VSOutput o;
	o.position = mul(proj, float4(input.aPos, 1.0f));
    o.TexCoord = input.aTexCoord;
    o.Normal   = input.aNormal;
    o.WorldPos = input.aPos;
    o.ViewPos  = float4(input.aPos, 1.0);
    return o;
}



[shader("fragment")]
vec4 fragMain(VSOutput input) : SV_Target
{
	vec4 texRaw = diffuseTexture.Sample(input.TexCoord);

	return texRaw;
}