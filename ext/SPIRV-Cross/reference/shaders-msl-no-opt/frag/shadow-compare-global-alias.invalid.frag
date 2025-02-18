#pragma clang diagnostic ignored "-Wmissing-prototypes"

#include <metal_stdlib>
#include <simd/simd.h>

using namespace metal;

struct main0_out
{
    float FragColor [[color(0)]];
};

struct main0_in
{
    float3 vUV [[user(locn0)]];
};

static inline __attribute__((always_inline))
float Samp(thread const float3& uv, depth2d<float> uTex, sampler uSamp)
{
    return uTex.sample_compare(uSamp, uv.xy, uv.z);
}

static inline __attribute__((always_inline))
float Samp2(thread const float3& uv, depth2d<float> uSampler, sampler uSamplerSmplr, thread float3& vUV)
{
    return uSampler.sample_compare(uSamplerSmplr, vUV.xy, vUV.z);
}

static inline __attribute__((always_inline))
float Samp3(depth2d<float> uT, sampler uS, thread const float3& uv, thread float3& vUV)
{
    return uT.sample_compare(uS, vUV.xy, vUV.z);
}

static inline __attribute__((always_inline))
float Samp4(depth2d<float> uS, sampler uSSmplr, thread const float3& uv, thread float3& vUV)
{
    return uS.sample_compare(uSSmplr, vUV.xy, vUV.z);
}

fragment main0_out main0(main0_in in [[stage_in]], depth2d<float> uTex [[texture(0)]], depth2d<float> uSampler [[texture(1)]], sampler uSamp [[sampler(0)]], sampler uSamplerSmplr [[sampler(1)]])
{
    main0_out out = {};
    out.FragColor = uSampler.sample_compare(uSamplerSmplr, in.vUV.xy, in.vUV.z);
    out.FragColor += uTex.sample_compare(uSamp, in.vUV.xy, in.vUV.z);
    float3 param = in.vUV;
    out.FragColor += Samp(param, uTex, uSamp);
    float3 param_1 = in.vUV;
    out.FragColor += Samp2(param_1, uSampler, uSamplerSmplr, in.vUV);
    float3 param_2 = in.vUV;
    out.FragColor += Samp3(uTex, uSamp, param_2, in.vUV);
    float3 param_3 = in.vUV;
    out.FragColor += Samp4(uSampler, uSamplerSmplr, param_3, in.vUV);
    return out;
}

