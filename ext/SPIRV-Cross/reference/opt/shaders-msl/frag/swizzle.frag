#include <metal_stdlib>
#include <simd/simd.h>

using namespace metal;

struct main0_out
{
    float4 FragColor [[color(0)]];
};

struct main0_in
{
    float3 vNormal [[user(locn1)]];
    float2 vUV [[user(locn2)]];
};

fragment main0_out main0(main0_in in [[stage_in]], texture2d<float> samp [[texture(0)]], sampler sampSmplr [[sampler(0)]])
{
    main0_out out = {};
    float4 _19 = samp.sample(sampSmplr, in.vUV);
    float _23 = _19.x;
    out.FragColor = float4(_23, _19.yz, 1.0);
    out.FragColor = float4(_23, _19.z, 1.0, 4.0);
    out.FragColor = float4(_23, _23, samp.sample(sampSmplr, (in.vUV + float2(0.100000001490116119384765625))).yy);
    out.FragColor = float4(in.vNormal, 1.0);
    out.FragColor = float4(in.vNormal + float3(1.7999999523162841796875), 1.0);
    out.FragColor = float4(in.vUV, in.vUV + float2(1.7999999523162841796875));
    return out;
}

