#include ".base.slang"
#include ".colors.slang"

typedef Input VSInput;
typedef Output VSOutput;

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
	vec3 color = applyAO(input.Normal, Color::WHITE);

    vec3 fogColor = Color::GRAY_FOG;

    color = applyFog(
        color, fogColor, input.ViewPos, DEFAULT_FOG_STRENGTH
    );

    return vec4(color, 1.0);
}
