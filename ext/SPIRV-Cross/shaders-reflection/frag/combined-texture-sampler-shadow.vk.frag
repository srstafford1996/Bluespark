#version 310 es
precision mediump float;

layout(set = 0, binding = 0) uniform mediump samplerShadow uSampler;
layout(set = 0, binding = 1) uniform mediump sampler uSampler1;
layout(set = 0, binding = 2) uniform texture2D uDepth;
layout(location = 0) out float FragColor;

float samp2(texture2D t, mediump samplerShadow s)
{
   return texture(sampler2DShadow(t, s), vec3(1.0));
}

float samp3(texture2D t, mediump sampler s)
{
   return texture(sampler2D(t, s), vec2(1.0)).x;
}

float samp(texture2D t, mediump samplerShadow s, mediump sampler s1)
{
   float r0 = samp2(t, s);
   float r1 = samp3(t, s1);
   return r0 + r1;
}

void main()
{
   FragColor = samp(uDepth, uSampler, uSampler1);
}
