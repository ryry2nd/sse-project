typedef float4x4 mat4;
typedef float3x3 mat3;

#if defined(VULKAN)
	#define BINDING_SRV(x, y)     [[vk::binding(x, y)]]
	#define BINDING_UAV(x, y)     [[vk::binding(x, y)]]
	#define BINDING_CBV(x, y)     [[vk::binding(x, y)]]
	#define BINDING_SAMPLER(x, y) [[vk::binding(x, y)]]
#elif defined(DX12)
	#define BINDING_SRV(x, y)     [[register(t##x, space##y)]]
	#define BINDING_UAV(x, y)     [[register(u##x, space##y)]]
	#define BINDING_CBV(x, y)     [[register(b##x, space##y)]]
	#define BINDING_SAMPLER(x, y) [[register(s##x, space##y)]]
#else
	// Read-only textures/buffers
	#define BINDING_SRV(x, y)
	// Read-write textures/buffers
	#define BINDING_UAV(x, y)
	// Constant buffers
	#define BINDING_CBV(x, y)
	// Samplers
	#define BINDING_SAMPLER(x, y)
#endif