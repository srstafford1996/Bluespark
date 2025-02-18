#version 310 es
precision mediump float;

layout(push_constant, std430) uniform PushConstants
{
   vec4 value0;
   vec4 value1;
} push;

layout(location = 0) in vec4 vColor;
layout(location = 0) out vec4 FragColor;

void main()
{
   FragColor = vColor + push.value0 + push.value1;
}
