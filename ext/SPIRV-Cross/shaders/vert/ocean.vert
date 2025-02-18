#version 310 es

#define YFLIP 0
#define SPECULAR 0
#define GLOSSMAP 0

#define DEBUG_NONE      0
#define DEBUG_DIFFUSE   1
#define DEBUG_SPECULAR  2
#define DEBUG_LIGHTING  3
#define DEBUG_FOG       4
#define DEBUG           DEBUG_NONE

#define FORWARD   0
#define DEFERRED  1
#define DEFERRED_VTEX 2

float saturate(float x) { return clamp(x, 0.0, 1.0); }

layout(std140, binding = 0) uniform GlobalVSData
{
    vec4 g_ViewProj_Row0;
    vec4 g_ViewProj_Row1;
    vec4 g_ViewProj_Row2;
    vec4 g_ViewProj_Row3;
    vec4 g_CamPos;
    vec4 g_CamRight;
    vec4 g_CamUp;
    vec4 g_CamFront;
    vec4 g_SunDir;
    vec4 g_SunColor;
    vec4 g_TimeParams;
    vec4 g_ResolutionParams;
    vec4 g_CamAxisRight;
    vec4 g_FogColor_Distance;
    vec4 g_ShadowVP_Row0;
    vec4 g_ShadowVP_Row1;
    vec4 g_ShadowVP_Row2;
    vec4 g_ShadowVP_Row3;
};

vec4 ComputeFogFactor(vec3 WorldPos)
{
    vec4 FogData;
    vec3 vEye = WorldPos - g_CamPos.xyz;
    vec3 nEye = normalize(vEye);
    FogData.w = exp(-dot(vEye, vEye) * g_FogColor_Distance.w * 0.75);

    float fog_sun_factor = pow(saturate(dot(nEye, g_SunDir.xyz)), 8.0);
    FogData.xyz = mix(vec3(1.0, 1.0, 1.0), vec3(0.6, 0.6, 0.9), nEye.y * 0.5 + 0.5);
    FogData.xyz = mix(FogData.xyz, vec3(0.95, 0.87, 0.78), fog_sun_factor);
    return FogData;
}

void ApplyFog(inout vec3 Color, vec4 FogData)
{
    Color = mix(FogData.xyz, Color, FogData.w);
}

void ApplyLighting(inout mediump vec3 Color, mediump float DiffuseFactor)
{
    mediump vec3 DiffuseLight = g_SunColor.xyz * DiffuseFactor;
    mediump vec3 AmbientLight = vec3(0.2, 0.35, 0.55) * 0.5;
    mediump vec3 Lighting = DiffuseLight + AmbientLight;
#if DEBUG == DEBUG_LIGHTING
    Color = Lighting;
#else
    Color *= Lighting;
#endif
}

void ApplySpecular(inout mediump vec3 Color, mediump vec3 EyeVec, mediump vec3 Normal, mediump vec3 SpecularColor, mediump float Shininess, mediump float FresnelAmount)
{
    mediump vec3 HalfAngle = normalize(-EyeVec + g_SunDir.xyz);

    mediump float v_dot_h = saturate(dot(HalfAngle, -EyeVec));
    mediump float n_dot_l = saturate(dot(Normal, g_SunDir.xyz));
    mediump float n_dot_h = saturate(dot(Normal, HalfAngle));
    mediump float n_dot_v = saturate(dot(-EyeVec, Normal));
    mediump float h_dot_l = saturate(dot(g_SunDir.xyz, HalfAngle));

    const mediump float roughness_value = 0.25;

    mediump float r_sq = roughness_value * roughness_value;
    mediump float n_dot_h_sq = n_dot_h * n_dot_h;
    mediump float roughness_a = 1.0 / (4.0 * r_sq * n_dot_h_sq * n_dot_h_sq);
    mediump float roughness_b = n_dot_h_sq - 1.0;
    mediump float roughness_c = r_sq * n_dot_h_sq;
    mediump float roughness = saturate(roughness_a * exp(roughness_b / roughness_c));

    FresnelAmount = 0.5;
    mediump float fresnel_term = pow(1.0 - n_dot_v, 5.0) * (1.0 - FresnelAmount) + FresnelAmount;

    mediump float geo_numerator = 2.0 * n_dot_h;
    mediump float geo_denominator = 1.0 / v_dot_h;
    mediump float geo_term = min(1.0, min(n_dot_v, n_dot_l) * geo_numerator * geo_denominator);

#if SPECULAR || GLOSSMAP
    Color += SpecularColor * g_SunColor.xyz * fresnel_term * roughness * n_dot_l * geo_term / (n_dot_v * n_dot_l + 0.0001);
#endif

    //Color = vec3(0.025 * 1.0 / (n_dot_v * n_dot_l));
}


precision highp int;

layout(binding = 0) uniform mediump sampler2D TexDisplacement;
layout(binding = 1) uniform mediump sampler2D TexLOD;

layout(location = 0) in vec4 Position;
layout(location = 1) in vec4 LODWeights;

layout(location = 0) out highp vec3 EyeVec;
layout(location = 1) out highp vec4 TexCoord;

layout(std140, binding = 4) uniform GlobalOcean
{
    vec4 OceanScale;
    vec4 OceanPosition;
    vec4 InvOceanSize_PatchScale;
    vec4 NormalTexCoordScale;
};

struct PatchData
{
    vec4 Position;
    vec4 LODs;
};

layout(std140, binding = 0) uniform Offsets
{
    PatchData Patches[256];
};

vec2 lod_factor(vec2 uv)
{
    float level = textureLod(TexLOD, uv, 0.0).x * (255.0 / 32.0);
    float floor_level = floor(level);
    float fract_level = level - floor_level;
    return vec2(floor_level, fract_level);
}

#ifdef VULKAN
#define INSTANCE_ID gl_InstanceIndex
#else
#define INSTANCE_ID gl_InstanceID
#endif

vec2 warp_position()
{
    float vlod = dot(LODWeights, Patches[INSTANCE_ID].LODs);
    vlod = mix(vlod, Patches[INSTANCE_ID].Position.w, all(equal(LODWeights, vec4(0.0))));

    float floor_lod = floor(vlod);
    float fract_lod = vlod - floor_lod;
    uint ufloor_lod = uint(floor_lod);

    uvec4 uPosition = uvec4(Position);
    uvec2 mask = (uvec2(1u) << uvec2(ufloor_lod, ufloor_lod + 1u)) - 1u;

    uvec4 rounding;
    rounding.x = uPosition.x < 32u ? mask.x : 0u;
    rounding.y = uPosition.y < 32u ? mask.x : 0u;
    rounding.z = uPosition.x < 32u ? mask.y : 0u;
    rounding.w = uPosition.y < 32u ? mask.y : 0u;

    //rounding = uPosition.xyxy * mask.xxyy;
    vec4 lower_upper_snapped = vec4((uPosition.xyxy + rounding) & (~mask).xxyy);
    return mix(lower_upper_snapped.xy, lower_upper_snapped.zw, fract_lod);
}

void main()
{
    vec2 PatchPos = Patches[INSTANCE_ID].Position.xz * InvOceanSize_PatchScale.zw;
    vec2 WarpedPos = warp_position();
    vec2 VertexPos = PatchPos + WarpedPos;
    vec2 NormalizedPos = VertexPos * InvOceanSize_PatchScale.xy;
    vec2 NormalizedTex = NormalizedPos * NormalTexCoordScale.zw;
    vec2 lod = lod_factor(NormalizedPos);
    vec2 Offset = exp2(lod.x) * InvOceanSize_PatchScale.xy * NormalTexCoordScale.zw;

    vec3 Displacement =
        mix(textureLod(TexDisplacement, NormalizedTex + 0.5 * Offset, lod.x).yxz,
            textureLod(TexDisplacement, NormalizedTex + 1.0 * Offset, lod.x + 1.0).yxz,
            lod.y);

    vec3 WorldPos = vec3(NormalizedPos.x, 0.0, NormalizedPos.y) + Displacement;
    WorldPos *= OceanScale.xyz;
    WorldPos += OceanPosition.xyz;

    EyeVec = WorldPos - g_CamPos.xyz;
    TexCoord = vec4(NormalizedTex, NormalizedTex * NormalTexCoordScale.xy) + 0.5 * InvOceanSize_PatchScale.xyxy * NormalTexCoordScale.zwzw;

    gl_Position = WorldPos.x * g_ViewProj_Row0 + WorldPos.y * g_ViewProj_Row1 + WorldPos.z * g_ViewProj_Row2 + g_ViewProj_Row3;
#if YFLIP
    gl_Position *= vec4(1.0, -1.0, 1.0, 1.0);
#endif
}

