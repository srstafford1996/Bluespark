#include <metal_stdlib>
#include <simd/simd.h>

using namespace metal;

struct TessLevels
{
    float inner0;
    float inner1;
    float outer0;
    float outer1;
    float outer2;
    float outer3;
};

kernel void main0(const device TessLevels& sb_levels [[buffer(0)]], uint3 gl_GlobalInvocationID [[thread_position_in_grid]], constant uint* spvIndirectParams [[buffer(29)]], device MTLTriangleTessellationFactorsHalf* spvTessLevel [[buffer(26)]])
{
    uint gl_PrimitiveID = min(gl_GlobalInvocationID.x / 1, spvIndirectParams[1] - 1);
    spvTessLevel[gl_PrimitiveID].insideTessellationFactor = half(sb_levels.inner0);
    spvTessLevel[gl_PrimitiveID].edgeTessellationFactor[0] = half(sb_levels.outer0);
    spvTessLevel[gl_PrimitiveID].edgeTessellationFactor[1] = half(sb_levels.outer1);
    spvTessLevel[gl_PrimitiveID].edgeTessellationFactor[2] = half(sb_levels.outer2);
}

