#version 450

#ifndef SPIRV_CROSS_CONSTANT_ID_201
#define SPIRV_CROSS_CONSTANT_ID_201 -10
#endif
const int _7 = SPIRV_CROSS_CONSTANT_ID_201;
const int _20 = (_7 + 2);
#ifndef SPIRV_CROSS_CONSTANT_ID_202
#define SPIRV_CROSS_CONSTANT_ID_202 100u
#endif
const uint _8 = SPIRV_CROSS_CONSTANT_ID_202;
const uint _25 = (_8 % 5u);
const int _30 = _7 - (-3) * (_7 / (-3));
const ivec4 _32 = ivec4(20, 30, _20, _30);
const ivec2 _34 = ivec2(_32.y, _32.x);
const int _35 = _32.y;
#ifndef SPIRV_CROSS_CONSTANT_ID_200
#define SPIRV_CROSS_CONSTANT_ID_200 3.141590118408203125
#endif
const float _9 = SPIRV_CROSS_CONSTANT_ID_200;

layout(location = 0) flat out int _4;

void main()
{
    vec4 pos = vec4(0.0);
    pos.y += float(_20);
    pos.z += float(_25);
    pos += vec4(_32);
    vec2 _58 = pos.xy + vec2(_34);
    pos = vec4(_58.x, _58.y, pos.z, pos.w);
    gl_Position = pos;
    _4 = _35;
}

