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
    float4 vInputs_0 [[attribute(0)]];
    float4 vInputs_1 [[attribute(1)]];
    float4 vInputs_2 [[attribute(2)]];
    float4 vInputs_3 [[attribute(3)]];
};

struct main0_patchIn
{
    float4 vBoo_0 [[attribute(4)]];
    float4 vBoo_1 [[attribute(5)]];
    float4 vBoo_2 [[attribute(6)]];
    float4 vBoo_3 [[attribute(7)]];
    int vIndex [[attribute(8)]];
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
    spvUnsafeArray<float4x4, 32> _16 = spvUnsafeArray<float4x4, 32>({ float4x4(patchIn.gl_in[0].vInputs_0, patchIn.gl_in[0].vInputs_1, patchIn.gl_in[0].vInputs_2, patchIn.gl_in[0].vInputs_3), float4x4(patchIn.gl_in[1].vInputs_0, patchIn.gl_in[1].vInputs_1, patchIn.gl_in[1].vInputs_2, patchIn.gl_in[1].vInputs_3), float4x4(patchIn.gl_in[2].vInputs_0, patchIn.gl_in[2].vInputs_1, patchIn.gl_in[2].vInputs_2, patchIn.gl_in[2].vInputs_3), float4x4(patchIn.gl_in[3].vInputs_0, patchIn.gl_in[3].vInputs_1, patchIn.gl_in[3].vInputs_2, patchIn.gl_in[3].vInputs_3), float4x4(patchIn.gl_in[4].vInputs_0, patchIn.gl_in[4].vInputs_1, patchIn.gl_in[4].vInputs_2, patchIn.gl_in[4].vInputs_3), float4x4(patchIn.gl_in[5].vInputs_0, patchIn.gl_in[5].vInputs_1, patchIn.gl_in[5].vInputs_2, patchIn.gl_in[5].vInputs_3), float4x4(patchIn.gl_in[6].vInputs_0, patchIn.gl_in[6].vInputs_1, patchIn.gl_in[6].vInputs_2, patchIn.gl_in[6].vInputs_3), float4x4(patchIn.gl_in[7].vInputs_0, patchIn.gl_in[7].vInputs_1, patchIn.gl_in[7].vInputs_2, patchIn.gl_in[7].vInputs_3), float4x4(patchIn.gl_in[8].vInputs_0, patchIn.gl_in[8].vInputs_1, patchIn.gl_in[8].vInputs_2, patchIn.gl_in[8].vInputs_3), float4x4(patchIn.gl_in[9].vInputs_0, patchIn.gl_in[9].vInputs_1, patchIn.gl_in[9].vInputs_2, patchIn.gl_in[9].vInputs_3), float4x4(patchIn.gl_in[10].vInputs_0, patchIn.gl_in[10].vInputs_1, patchIn.gl_in[10].vInputs_2, patchIn.gl_in[10].vInputs_3), float4x4(patchIn.gl_in[11].vInputs_0, patchIn.gl_in[11].vInputs_1, patchIn.gl_in[11].vInputs_2, patchIn.gl_in[11].vInputs_3), float4x4(patchIn.gl_in[12].vInputs_0, patchIn.gl_in[12].vInputs_1, patchIn.gl_in[12].vInputs_2, patchIn.gl_in[12].vInputs_3), float4x4(patchIn.gl_in[13].vInputs_0, patchIn.gl_in[13].vInputs_1, patchIn.gl_in[13].vInputs_2, patchIn.gl_in[13].vInputs_3), float4x4(patchIn.gl_in[14].vInputs_0, patchIn.gl_in[14].vInputs_1, patchIn.gl_in[14].vInputs_2, patchIn.gl_in[14].vInputs_3), float4x4(patchIn.gl_in[15].vInputs_0, patchIn.gl_in[15].vInputs_1, patchIn.gl_in[15].vInputs_2, patchIn.gl_in[15].vInputs_3), float4x4(patchIn.gl_in[16].vInputs_0, patchIn.gl_in[16].vInputs_1, patchIn.gl_in[16].vInputs_2, patchIn.gl_in[16].vInputs_3), float4x4(patchIn.gl_in[17].vInputs_0, patchIn.gl_in[17].vInputs_1, patchIn.gl_in[17].vInputs_2, patchIn.gl_in[17].vInputs_3), float4x4(patchIn.gl_in[18].vInputs_0, patchIn.gl_in[18].vInputs_1, patchIn.gl_in[18].vInputs_2, patchIn.gl_in[18].vInputs_3), float4x4(patchIn.gl_in[19].vInputs_0, patchIn.gl_in[19].vInputs_1, patchIn.gl_in[19].vInputs_2, patchIn.gl_in[19].vInputs_3), float4x4(patchIn.gl_in[20].vInputs_0, patchIn.gl_in[20].vInputs_1, patchIn.gl_in[20].vInputs_2, patchIn.gl_in[20].vInputs_3), float4x4(patchIn.gl_in[21].vInputs_0, patchIn.gl_in[21].vInputs_1, patchIn.gl_in[21].vInputs_2, patchIn.gl_in[21].vInputs_3), float4x4(patchIn.gl_in[22].vInputs_0, patchIn.gl_in[22].vInputs_1, patchIn.gl_in[22].vInputs_2, patchIn.gl_in[22].vInputs_3), float4x4(patchIn.gl_in[23].vInputs_0, patchIn.gl_in[23].vInputs_1, patchIn.gl_in[23].vInputs_2, patchIn.gl_in[23].vInputs_3), float4x4(patchIn.gl_in[24].vInputs_0, patchIn.gl_in[24].vInputs_1, patchIn.gl_in[24].vInputs_2, patchIn.gl_in[24].vInputs_3), float4x4(patchIn.gl_in[25].vInputs_0, patchIn.gl_in[25].vInputs_1, patchIn.gl_in[25].vInputs_2, patchIn.gl_in[25].vInputs_3), float4x4(patchIn.gl_in[26].vInputs_0, patchIn.gl_in[26].vInputs_1, patchIn.gl_in[26].vInputs_2, patchIn.gl_in[26].vInputs_3), float4x4(patchIn.gl_in[27].vInputs_0, patchIn.gl_in[27].vInputs_1, patchIn.gl_in[27].vInputs_2, patchIn.gl_in[27].vInputs_3), float4x4(patchIn.gl_in[28].vInputs_0, patchIn.gl_in[28].vInputs_1, patchIn.gl_in[28].vInputs_2, patchIn.gl_in[28].vInputs_3), float4x4(patchIn.gl_in[29].vInputs_0, patchIn.gl_in[29].vInputs_1, patchIn.gl_in[29].vInputs_2, patchIn.gl_in[29].vInputs_3), float4x4(patchIn.gl_in[30].vInputs_0, patchIn.gl_in[30].vInputs_1, patchIn.gl_in[30].vInputs_2, patchIn.gl_in[30].vInputs_3), float4x4(patchIn.gl_in[31].vInputs_0, patchIn.gl_in[31].vInputs_1, patchIn.gl_in[31].vInputs_2, patchIn.gl_in[31].vInputs_3) });
    spvUnsafeArray<float4x4, 32> tmp;
    tmp = _16;
    out.gl_Position = (tmp[0][patchIn.vIndex] + tmp[1][patchIn.vIndex]) + vBoo[patchIn.vIndex];
    return out;
}

