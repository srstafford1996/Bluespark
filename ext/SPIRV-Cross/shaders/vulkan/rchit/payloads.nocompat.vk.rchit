#version 460
#extension GL_NV_ray_tracing : require

struct Payload
{
	vec4 a;	
};

layout(location = 0) rayPayloadInNV Payload payload;

void write_incoming_payload_in_function()
{
	payload.a = vec4(10.0);
}

void main()
{
	write_incoming_payload_in_function();
}
