struct VSInput
{
	float3 aPos      : POSITION;
	float2 aTexCoord : TEXCOORD0;
};

struct VSOutput
{
	float2 TexCoord  : TEXCOORD0;
	float4 position  : SV_Position;
};

struct PSInput
{
	float2 TexCoord : TEXCOORD0;
};

struct Camera
{
	float4x4 view;
	float4x4 proj;
};

struct Model
{
	float4x4 model;
};

ConstantBuffer<Camera> camera;
ConstantBuffer<Model> model;

// =======================
// VERTEX ENTRY
// =======================
[shader("vertex")]
VSOutput vertMain(VSInput input)
{
	VSOutput output;

	output.TexCoord = input.aTexCoord;

	float4 worldPos = mul(model.model, float4(input.aPos, 1.0));
	output.position = mul(camera.proj, mul(camera.view, worldPos));

	return output;
}

// =======================
// FRAGMENT ENTRY
// =======================
[shader("fragment")]
float4 fragMain(PSInput input) : SV_Target
{
	return float4(input.TexCoord, 0.0, 1.0);
}