#pragma clang diagnostic ignored "-Wmissing-prototypes"

#include <metal_stdlib>
#include <simd/simd.h>

using namespace metal;

struct main0_out
{
    float4 FragColor [[color(0)]];
};

struct main0_in
{
    float2 vUV [[user(locn0)]];
};

static inline __attribute__((always_inline))
float4 foo(texture2d<float> uSampler, sampler uSamplerSmplr, thread float2& vUV)
{
    float4 color;
    if (!simd_is_helper_thread())
    {
        color = uSampler.sample(uSamplerSmplr, vUV, level(0.0));
    }
    else
    {
        color = float4(1.0);
    }
    return color;
}

fragment main0_out main0(main0_in in [[stage_in]], texture2d<float> uSampler [[texture(0)]], sampler uSamplerSmplr [[sampler(0)]])
{
    main0_out out = {};
    out.FragColor = foo(uSampler, uSamplerSmplr, in.vUV);
    return out;
}

