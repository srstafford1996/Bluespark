#version 310 es
precision mediump float;

layout(set = 0, binding = 0) uniform mediump sampler uSampler;
layout(set = 0, binding = 1) uniform mediump texture2D uTexture[4];
layout(set = 0, binding = 2) uniform mediump texture3D uTexture3D[4];
layout(set = 0, binding = 3) uniform mediump textureCube uTextureCube[4];
layout(set = 0, binding = 4) uniform mediump texture2DArray uTextureArray[4];

layout(location = 0) out vec4 FragColor;
layout(location = 0) in vec2 vTex;
layout(location = 1) in vec3 vTex3;

vec4 sample_func(mediump sampler samp, vec2 uv)
{
    return texture(sampler2D(uTexture[2], samp), uv);
}

vec4 sample_func_dual(mediump sampler samp, mediump texture2D tex, vec2 uv)
{
    return texture(sampler2D(tex, samp), uv);
}

vec4 sample_func_dual_array(mediump sampler samp, mediump texture2D tex[4], vec2 uv)
{
    return texture(sampler2D(tex[1], samp), uv);
}

void main()
{
    vec2 off = 1.0 / vec2(textureSize(sampler2D(uTexture[1], uSampler), 0));
    vec2 off2 = 1.0 / vec2(textureSize(sampler2D(uTexture[2], uSampler), 1));

    vec4 c0 = sample_func(uSampler, vTex + off + off2);
    vec4 c1 = sample_func_dual(uSampler, uTexture[1], vTex + off + off2);
    vec4 c2 = sample_func_dual_array(uSampler, uTexture, vTex + off + off2);
    vec4 c3 = texture(sampler2DArray(uTextureArray[3], uSampler), vTex3);
    vec4 c4 = texture(samplerCube(uTextureCube[1], uSampler), vTex3);
    vec4 c5 = texture(sampler3D(uTexture3D[2], uSampler), vTex3);

    FragColor = c0 + c1 + c2 + c3 + c4 + c5;
}
