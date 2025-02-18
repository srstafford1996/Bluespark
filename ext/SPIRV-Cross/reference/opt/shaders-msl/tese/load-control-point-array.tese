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
    float4 gl_Position [[position]];
};

struct main0_in
{
    float4 vInputs [[attribute(0)]];
};

struct main0_patchIn
{
    float4 vBoo_0 [[attribute(1)]];
    float4 vBoo_1 [[attribute(2)]];
    float4 vBoo_2 [[attribute(3)]];
    float4 vBoo_3 [[attribute(4)]];
    int vIndex [[attribute(5)]];
    patch_control_point<main0_in> gl_in;
};

[[ patch(quad, 0) ]] vertex main0_out main0(main0_patchIn patchIn [[stage_in]])
{
    main0_out out = {};
    spvUnsafeArray<float4, 4> vBoo = {};
    vBoo[0] = patchIn.vBoo_0;
    vBoo[1] = patchIn.vBoo_1;
    vBoo[2] = patchIn.vBoo_2;
    vBoo[3] = patchIn.vBoo_3;
    out.gl_Position = (patchIn.gl_in[0u].vInputs + patchIn.gl_in[1u].vInputs) + vBoo[patchIn.vIndex];
    return out;
}

