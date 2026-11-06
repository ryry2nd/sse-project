typedef float4x4 mat4;
typedef float3x3 mat3;
typedef float2 vec2;
typedef float3 vec3;
typedef float4 vec4;

typedef uint32_t Uint32;
typedef uint16_t Uint16;
typedef uint8_t Uint8;

#if defined(VULKAN) || defined(GL)
	#define BINDING_SRV(x)     [[vk::binding(x, 0)]]
	#define BINDING_UAV(x)     [[vk::binding(x, 1)]]
	#define BINDING_CBV(x)     [[vk::binding(x, 2)]]
	#define BINDING_IMG(x)     [[vk::binding(x, 3)]]
	#define BINDING_SAMPLER(x) [[vk::binding(x, 4)]]
	#define BINDING_IMGRW(x)     [[vk::binding(x, 5)]]
#elif defined(DX12)
	#define BINDING_SRV(x)     [[register(t##x, space0)]]
	#define BINDING_UAV(x)     [[register(u##x, space1)]]
	#define BINDING_CBV(x)     [[register(b##x, space2)]]
	#define BINDING_IMG(x)     [[register(t##x, space3)]]
	#define BINDING_SAMPLER(x) [[register(s##x, space4)]]
	#define BINDING_IMGRW(x)   [[register(u##x, space5)]]
#else
	// Read-only buffers
	#define BINDING_SRV(x)
	// Read-write buffers
	#define BINDING_UAV(x)
	// Constant buffers
	#define BINDING_CBV(x)
	// Textures
	#define BINDING_IMG(x)
	// Samplers
	#define BINDING_SAMPLER(x)
	// RWTextures
	#define BINDING_IMGRW(x)
#endif

#define DEFAULT_FOG_STRENGTH 0.003f

#define DEFAULT_EDGE_INNER 0.2f
#define DEFAULT_EDGE_OUTER 0.5f
#define DEFAULT_EDGE_STRENGTH 0.15f


static const float PI = float.getPi();

namespace Structs {
	struct Input
	{
		vec3 aPos      : POSITION;
		vec2 aTexCoord : TEXCOORD0;
		vec3 aNormal   : NORMAL;
	};

	struct Vert
	{
		vec3 aPos;
		vec2 aTexCoord;
		vec3 aNormal;
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

	struct Material
	{
		float3 ambient;
		float pad0;

		float3 diffuse;
		float pad1;

		float3 specular;
		float pad2;

		float3 emissive;
		float shininess;

		float opacity;
		float ior;
		float pad3;
		float pad4;
	};

	typedef mat4 Model;
}

namespace Normals {
	vec3 transformNormal(vec3 normal, mat4 model)
	{
		return normalize(mul((mat3)model, normal));
	}

	vec3 SafeNormalize(vec3 v, vec3 fallback)
	{
		float lenSq = dot(v, v);

		if (lenSq < 0.000001)
			return fallback;

		return v * rsqrt(lenSq);
	}

	vec3 GetNormalFromMap(vec3 normal, vec3 position, vec2 uv, Sampler2D bumpTexture)
	{
		vec3 N = SafeNormalize(normal, vec3(0.0, 0.0, 1.0));

		vec3 dp1 = ddx(position);
		vec3 dp2 = ddy(position);

		vec2 duv1 = ddx(uv);
		vec2 duv2 = ddy(uv);

		vec3 T = dp1 * duv2.y - dp2 * duv1.y;
		vec3 B = -dp1 * duv2.x + dp2 * duv1.x;

		T = T - N * dot(N, T);
		T = SafeNormalize(T, vec3(1.0, 0.0, 0.0));

		// Preserve UV orientation.
		float handedness = dot(cross(N, T), B) < 0.0 ? -1.0 : 1.0;
		B = cross(N, T) * handedness;

		vec3 normalMap = bumpTexture.Sample(uv).rgb;
		normalMap = normalMap * 2.0 - 1.0;

		return SafeNormalize(
			T * normalMap.x +
			B * normalMap.y +
			N * normalMap.z,
			N
		);
	}
}

namespace Fog {
	float fogExp2(float dist, float density)
	{
		return exp(-(dist * density) * (dist * density));
	}


	vec3 applyFog(vec3 inColor, vec3 fogColor, vec4 viewPos, float density) {
		float dist = length(viewPos.xyz);
		float fog = fogExp2(dist, density);
		return lerp(fogColor, inColor, fog);
	}
}

namespace Random {
	float2 randomGradient(int2 p, float seed)
	{
		uint2 q = asuint(p);

		uint h = q.x * 374761393u
			+ q.y * 668265263u
			+ (uint)seed * 1442695041u;

		h = (h ^ (h >> 13)) * 1274126177u;
		h ^= h >> 16;

		float angle = (float)(h & 0x00FFFFFFu) / 16777216.0
					* 6.28318530718;

		return float2(cos(angle), sin(angle));
	}

	float perlinNoise(float2 p, float seed)
	{
		int2 i = int2(floor(p));
		float2 f = frac(p);

		float2 u = f * f * f * (f * (f * 6.0 - 15.0) + 10.0);

		float n00 = dot(randomGradient(i + int2(0, 0), seed), f - float2(0, 0));
		float n10 = dot(randomGradient(i + int2(1, 0), seed), f - float2(1, 0));
		float n01 = dot(randomGradient(i + int2(0, 1), seed), f - float2(0, 1));
		float n11 = dot(randomGradient(i + int2(1, 1), seed), f - float2(1, 1));

		float nx0 = lerp(n00, n10, u.x);
		float nx1 = lerp(n01, n11, u.x);

		return lerp(nx0, nx1, u.y);
	}

	float random(float2 p)
	{
		float3 p3 = frac(float3(p.xyx) * 0.1031);
		p3 += dot(p3, p3.yzx + 33.33);
		return frac((p3.x + p3.y) * p3.z);
	}
}

namespace Utils {
	vec3 applyAO(vec3 normal, vec3 color) {
		vec3 N = normalize(normal);
		float shade = 0.65 + 0.35 * abs(N.y);
		return color * shade;
	}

	float edgeDarken(vec2 texCoord, float inner, float outer, float strength)
	{
		vec2 uv = vec2(frac(texCoord.x), frac(texCoord.y)) - 0.5f;
		float edge = length(uv);
		float dark = smoothstep(inner, outer, edge);
		return 1.0 - dark * strength;
	}

	float2 rotateUV(float2 uv, uint rotations)
	{
		rotations %= 4;

		for (uint i = 0; i < rotations; i++)
		{
			uv = float2(uv.y, 1.0 - uv.x);
		}

		return uv;
	}

	Structs::Output getPosition(mat4 model, mat4 view, mat4 proj, Structs::Input input) {
		Structs::Output output;

		vec4 worldPos = mul(model, vec4(input.aPos, 1.0));

		output.WorldPos = worldPos.xyz;

		output.Normal = Normals::transformNormal(input.aNormal, model);

		output.TexCoord = input.aTexCoord;

		output.ViewPos = mul(view, worldPos);

		output.position = mul(proj, output.ViewPos);

		return output;
	}
}