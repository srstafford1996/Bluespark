#include <metal_stdlib>
#include <simd/simd.h>

using namespace metal;

struct main0_out
{
    float4 result [[color(0)]];
};

struct main0_in
{
    float4 accum [[user(locn0)]];
};

fragment main0_out main0(main0_in in [[stage_in]])
{
    main0_out out = {};
    out.result = float4(0.0);
    uint j;
    for (int i = 0; i < 4; i += int(j))
    {
        if (in.accum.y > 10.0)
        {
            j = 40u;
        }
        else
        {
            j = 30u;
        }
        out.result += in.accum;
    }
    return out;
}

