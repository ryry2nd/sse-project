typedef float4x4 mat4;
typedef float3x3 mat3;
typedef float2 vec2;
typedef float3 vec3;
typedef float4 vec4;

#if defined(VULKAN) || defined(GL)
	#define BINDING_SRV(x)     [[vk::binding(x, 0)]]
	#define BINDING_UAV(x)     [[vk::binding(x, 1)]]
	#define BINDING_CBV(x)     [[vk::binding(x, 2)]]
	#define BINDING_IMG(x)     [[vk::binding(x, 3)]]
	#define BINDING_SAMPLER(x) [[vk::binding(x, 4)]]
#elif defined(DX12)
	#define BINDING_SRV(x)     [[register(t##x, space0)]]
	#define BINDING_UAV(x)     [[register(u##x, space1)]]
	#define BINDING_CBV(x)     [[register(b##x, space2)]]
	#define BINDING_IMG(x)     [[register(t##x, space3)]]
	#define BINDING_SAMPLER(x) [[register(s##x, space4)]]
#else
	// Read-only buffers
	#define BINDING_SRV(x)
	// Read-write buffers
	#define BINDING_UAV(x)
	// Constant buffers
	#define BINDING_CBV(x)

	// Textures/Samplers
	#define BINDING_IMG(x)
	#define BINDING_SAMPLER(x)
#endif


struct Input
{
	vec3 aPos      : POSITION;
	vec2 aTexCoord : TEXCOORD0;
	vec3 aNormal   : NORMAL;
};

struct Output
{
	vec2 TexCoord;
	vec3 Normal;
	vec3 WorldPos;
	vec4 ViewPos;
	vec4 position : SV_Position;
};

struct Camera
{
	mat4 view;
	mat4 proj;
};

typedef mat4 Model;

vec3 transformNormal(vec3 normal, mat4 model)
{
    return normalize(mul((mat3)model, normal));
}

float edgeDarken(vec2 texCoord, float inner, float outer, float strength)
{
	vec2 uv = texCoord - 0.5f;
    float edge = length(uv);
    float dark = smoothstep(inner, outer, edge);
    return 1.0 - dark * strength;
}


float fogExp2(float dist, float density)
{
    return exp(-(dist * density) * (dist * density));
}


vec3 applyFog(vec3 inColor, vec3 fogColor, vec4 viewPos, float density) {
	float dist = length(viewPos.xyz);
	float fog = fogExp2(dist, density);
	return lerp(fogColor, inColor, fog);
}

Output getPosition(mat4 model, mat4 view, mat4 proj, Input input) {
	Output output;

	vec4 worldPos = mul(model, vec4(input.aPos, 1.0));

	output.WorldPos = worldPos.xyz;

	output.Normal = transformNormal(input.aNormal, model);

	output.TexCoord = input.aTexCoord;

	output.ViewPos = mul(view, worldPos);

	output.position = mul(proj, output.ViewPos);

	return output;
}