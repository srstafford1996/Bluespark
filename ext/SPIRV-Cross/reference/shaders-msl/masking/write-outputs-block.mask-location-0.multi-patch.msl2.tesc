#pragma clang diagnostic ignored "-Wmissing-prototypes"

#include <metal_stdlib>
#include <simd/simd.h>

using namespace metal;

struct P
{
    float a;
    float b;
};

struct C
{
    float a;
    float b;
};

struct main0_out
{
    float c_a;
    float c_b;
    float4 gl_Position;
};

struct main0_patchOut
{
    float m_11_b;
};

static inline __attribute__((always_inline))
void write_in_function(threadgroup P& _11, device main0_patchOut& patchOut, device main0_out* thread & gl_out, thread uint& gl_InvocationID)
{
    _11.a = 1.0;
    patchOut.m_11_b = 2.0;
    gl_out[gl_InvocationID].c_a = 3.0;
    gl_out[gl_InvocationID].c_b = 4.0;
    gl_out[gl_InvocationID].gl_Position = float4(1.0);
}

kernel void main0(uint3 gl_GlobalInvocationID [[thread_position_in_grid]], device main0_out* spvOut [[buffer(28)]], constant uint* spvIndirectParams [[buffer(29)]], device main0_patchOut* spvPatchOut [[buffer(27)]], device MTLQuadTessellationFactorsHalf* spvTessLevel [[buffer(26)]])
{
    device main0_out* gl_out = &spvOut[gl_GlobalInvocationID.x - gl_GlobalInvocationID.x % 4];
    device main0_patchOut& patchOut = spvPatchOut[gl_GlobalInvocationID.x / 4];
    threadgroup P spvStorage_11[8];
    threadgroup P (&_11) = spvStorage_11[(gl_GlobalInvocationID.x / 4) % 8];
    uint gl_InvocationID = gl_GlobalInvocationID.x % 4;
    uint gl_PrimitiveID = min(gl_GlobalInvocationID.x / 4, spvIndirectParams[1] - 1);
    write_in_function(_11, patchOut, gl_out, gl_InvocationID);
}

