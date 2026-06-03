#include ".base.slang"
#include ".colors.slang"

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
	vec4 texRaw = diffuseTexture.Sample(input.TexCoord);

	float edgeFactor = edgeDarken(input.TexCoord, 0.2f, 0.5f, 0.15f);


	vec3 color = texRaw.xyz * edgeFactor;
	vec3 fogColor = Color::GRAY_FOG;

	return vec4(applyFog(color, fogColor, input.ViewPos, 0.006), 1);
}