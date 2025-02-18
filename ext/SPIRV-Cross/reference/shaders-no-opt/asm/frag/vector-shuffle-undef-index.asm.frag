#version 450

vec4 undef;

layout(location = 0) out vec4 FragColor;
layout(location = 0) in vec4 vFloat;

void main()
{
    FragColor = vec4(undef.x, vFloat.y, 0.0, vFloat.w) + vec4(vFloat.z, vFloat.y, 0.0, vFloat.w);
}

