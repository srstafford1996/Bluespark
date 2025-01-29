#version 460
#extension GL_NV_ray_tracing : require

layout(location = 0) rayPayloadInNV Foo { float a; float b; } payload;
hitAttributeNV Foo2 { float a; float b; } hit;

void main()
{
	payload.a = hit.a;
	payload.b = hit.b;
}
