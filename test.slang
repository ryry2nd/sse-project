struct VSIn
{
    float3 pos : POSITION;
};

struct VSOut
{
    float4 pos : SV_Position;
};

VSOut vertMain(VSIn input)
{
    VSOut o;
    o.pos = float4(input.pos, 1.0);
    return o;
}

float4 fragMain() : SV_Target
{
    return float4(1, 0, 0, 1);
}
