#version 460
#extension GL_EXT_ray_tracing : require

layout(set = 0, binding = 0) uniform accelerationStructureEXT as;
layout(location = 3) rayPayloadInEXT float p;

void main()
{
    vec3 origin = vec3(float(gl_LaunchIDEXT.x) / float(gl_LaunchSizeEXT.x), float(gl_LaunchIDEXT.y) / float(gl_LaunchSizeEXT.y), 1.0);
    vec3 direction = vec3(0.0, 0.0, -1.0);
    traceRayEXT(as, 0u, 255u, 0u, 1u, 0u, origin, 0.0, direction, 1000.0, 3);
}
