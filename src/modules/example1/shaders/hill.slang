#include ".base.slang"
#include ".colors.slang"

using namespace Structs;
using namespace Normals;
using namespace Utils;
using namespace Fog;
using namespace Random;

typedef Input VSInput;
typedef Output VSOutput;


BINDING_CBV(0)
ConstantBuffer<Model> model;

BINDING_CBV(1)
ConstantBuffer<Camera> cam;

BINDING_IMG(0)
Sampler2D diffuseTexture;


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

	vec3 color = texRaw.xyz;
	vec3 fogColor = Color::GRAY_FOG;

	if (texRaw.a < 0.5)
    	discard;

	return vec4(applyFog(color, fogColor, input.ViewPos, DEFAULT_FOG_STRENGTH), texRaw.a);
}