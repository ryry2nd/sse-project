#include ".base.slang"

BINDING_SRV(0)
RWStructuredBuffer<uint> outputBuffer;

[shader("compute")]
[numthreads(256, 1, 1)]
void computeMain(uint3 id : SV_DispatchThreadID)
{
    outputBuffer[id.x] = id.x * 2;
}