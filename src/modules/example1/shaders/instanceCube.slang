#include ".colors.slang"
#include ".base.slang"

typedef Input VSInput;
typedef Output VSOutput;


BINDING_CBV(0)
ConstantBuffer<Camera> cam;

BINDING_SRV(0)
StructuredBuffer<Model> models;



[shader("vertex")]
VSOutput vertMain(VSInput input, uint instanceID : SV_InstanceID)
{
	mat4 proj = cam.proj;
	mat4 view = cam.view;
	mat4 model = models[instanceID];

	return getPosition(model, view, proj, input);
}



[shader("fragment")]
vec4 fragMain(VSOutput input) : SV_Target
{
	float edgeFactor = edgeDarken(input.TexCoord, 0.2f, 0.5f, 0.15f);


	vec3 color = Color::GREEN * edgeFactor;
	vec3 fogColor = Color::GRAY_FOG;

	return vec4(applyFog(color, fogColor, input.ViewPos, 0.006), 1.0f);
}