struct VSInput
{
    float3 aPos      : POSITION;
};

struct VSOutput
{
    float4 position : SV_Position;
};

cbuffer Model
{
    float4x4 model;
};

cbuffer Camera
{
    float4x4 view;
    float4x4 proj;
};

// =======================
// VERTEX
// =======================
[shader("vertex")]
VSOutput vertMain(VSInput input)
{
    VSOutput output;

    float4 worldPos = mul(model, float4(input.aPos, 1.0));
    output.position = mul(proj, mul(view, worldPos));

    return output;
}

// =======================
// FRAGMENT
// =======================
[shader("fragment")]
float4 fragMain() : SV_Target
{
    return float4(0, 1, 0, 1);
}