#version 450
#ifdef GL_ARB_fragment_shader_interlock
#extension GL_ARB_fragment_shader_interlock : enable
#define SPIRV_Cross_beginInvocationInterlock() beginInvocationInterlockARB()
#define SPIRV_Cross_endInvocationInterlock() endInvocationInterlockARB()
#elif defined(GL_INTEL_fragment_shader_ordering)
#extension GL_INTEL_fragment_shader_ordering : enable
#define SPIRV_Cross_beginInvocationInterlock() beginFragmentShaderOrderingINTEL()
#define SPIRV_Cross_endInvocationInterlock()
#endif
#if defined(GL_ARB_fragment_shader_interlock)
layout(pixel_interlock_unordered) in;
#elif !defined(GL_INTEL_fragment_shader_ordering)
#error Fragment Shader Interlock/Ordering extension missing!
#endif

layout(binding = 2, std430) coherent buffer Buffer
{
    int foo;
    uint bar;
} _30;

layout(binding = 0, rgba8) uniform writeonly image2D img;
layout(binding = 1, r32ui) uniform uimage2D img2;

void main()
{
    SPIRV_Cross_beginInvocationInterlock();
    imageStore(img, ivec2(0), vec4(1.0, 0.0, 0.0, 1.0));
    uint _27 = imageAtomicAdd(img2, ivec2(0), 1u);
    _30.foo += 42;
    uint _41 = atomicAnd(_30.bar, 255u);
    SPIRV_Cross_endInvocationInterlock();
}

