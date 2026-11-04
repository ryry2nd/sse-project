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
	float edgeFactor = edgeDarken(input.TexCoord, DEFAULT_EDGE_INNER, DEFAULT_EDGE_OUTER, DEFAULT_EDGE_STRENGTH);

	vec3 rawColor = vec3(1.0f, 1.0f, 1.0f);
	vec3 color = rawColor * edgeFactor;
	vec3 fogColor = Color::GRAY_FOG;

	return vec4(applyFog(color, fogColor, input.ViewPos, DEFAULT_FOG_STRENGTH), 1.0f);
}