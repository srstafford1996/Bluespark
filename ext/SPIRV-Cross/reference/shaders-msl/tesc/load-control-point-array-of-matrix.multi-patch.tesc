#pragma clang diagnostic ignored "-Wmissing-prototypes"
#pragma clang diagnostic ignored "-Wmissing-braces"

#include <metal_stdlib>
#include <simd/simd.h>

using namespace metal;

template<typename T, size_t Num>
struct spvUnsafeArray
{
    T elements[Num ? Num : 1];
    
    thread T& operator [] (size_t pos) thread
    {
        return elements[pos];
    }
    constexpr const thread T& operator [] (size_t pos) const thread
    {
        return elements[pos];
    }
    
    device T& operator [] (size_t pos) device
    {
        return elements[pos];
    }
    constexpr const device T& operator [] (size_t pos) const device
    {
        return elements[pos];
    }
    
    constexpr const constant T& operator [] (size_t pos) const constant
    {
        return elements[pos];
    }
    
    threadgroup T& operator [] (size_t pos) threadgroup
    {
        return elements[pos];
    }
    constexpr const threadgroup T& operator [] (size_t pos) const threadgroup
    {
        return elements[pos];
    }
};

struct main0_out
{
    float4x4 vOutputs;
};

struct main0_in
{
    float4x4 vInputs;
};

kernel void main0(uint3 gl_GlobalInvocationID [[thread_position_in_grid]], device main0_out* spvOut [[buffer(28)]], constant uint* spvIndirectParams [[buffer(29)]], device MTLQuadTessellationFactorsHalf* spvTessLevel [[buffer(26)]], device main0_in* spvIn [[buffer(22)]])
{
    device main0_out* gl_out = &spvOut[gl_GlobalInvocationID.x - gl_GlobalInvocationID.x % 4];
    device main0_in* gl_in = &spvIn[min(gl_GlobalInvocationID.x / 4, spvIndirectParams[1] - 1) * spvIndirectParams[0]];
    uint gl_InvocationID = gl_GlobalInvocationID.x % 4;
    uint gl_PrimitiveID = min(gl_GlobalInvocationID.x / 4, spvIndirectParams[1] - 1);
    spvUnsafeArray<float4x4, 32> _16 = spvUnsafeArray<float4x4, 32>({ gl_in[0].vInputs, gl_in[1].vInputs, gl_in[2].vInputs, gl_in[3].vInputs, gl_in[4].vInputs, gl_in[5].vInputs, gl_in[6].vInputs, gl_in[7].vInputs, gl_in[8].vInputs, gl_in[9].vInputs, gl_in[10].vInputs, gl_in[11].vInputs, gl_in[12].vInputs, gl_in[13].vInputs, gl_in[14].vInputs, gl_in[15].vInputs, gl_in[16].vInputs, gl_in[17].vInputs, gl_in[18].vInputs, gl_in[19].vInputs, gl_in[20].vInputs, gl_in[21].vInputs, gl_in[22].vInputs, gl_in[23].vInputs, gl_in[24].vInputs, gl_in[25].vInputs, gl_in[26].vInputs, gl_in[27].vInputs, gl_in[28].vInputs, gl_in[29].vInputs, gl_in[30].vInputs, gl_in[31].vInputs });
    spvUnsafeArray<float4x4, 32> tmp;
    tmp = _16;
    gl_out[gl_InvocationID].vOutputs = tmp[gl_InvocationID];
}

