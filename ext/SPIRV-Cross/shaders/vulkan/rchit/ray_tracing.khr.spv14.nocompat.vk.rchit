#version 460
#extension GL_EXT_ray_tracing : require

layout(location = 0) rayPayloadInEXT float payload;

void main()
{
    payload = 1.0 + float(gl_InstanceID);
}
