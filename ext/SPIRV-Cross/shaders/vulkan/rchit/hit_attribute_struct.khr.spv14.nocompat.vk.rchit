#version 460
#extension GL_EXT_ray_tracing : require

struct Foo { float a; float b; };

layout(location = 0) rayPayloadInEXT Foo payload;
hitAttributeEXT Foo hit;

void main()
{
	payload = hit;
}
