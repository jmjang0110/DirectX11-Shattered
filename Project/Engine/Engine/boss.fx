#ifndef _BOSS_EFFECT
#define _BOSS_EFFECT

#include "value.fx"

struct VTX_IN
{
    float3 vPos : POSITION;
    float2 vUV : TEXCOORD;

    float3 vTangent : TANGENT;
    float3 vNormal : NORMAL;
    float3 vBinormal : BINORMAL;

    float4 vWeights : BLENDWEIGHT;
    float4 vIndices : BLENDINDICES;
};

struct VTX_OUT
{
    float4 vPosition : SV_Position;
    float2 vUV : TEXCOORD;

    float3 vViewPos : POSITION;
    float3 vViewTangent : TANGENT;
    float3 vViewNormal : NORMAL;
    float3 vViewBinormal : BINORMAL;
};

// =========================
// Column Flame Shader
// DOMAIN : DOMAIN_DEFERRED
// Rasterizer : CULL_NONE
// DepthStencilState : LESS
// BlendState : DEFAULT
// =========================

#define YPOS g_float_0
#define COLOR g_vec4_0
#define SCALE g_vec4_1

VTX_OUT VS_ColumnFlame(VTX_IN _in)
{
    VTX_OUT output = (VTX_OUT) 0.f;

    output.vPosition = mul(float4(_in.vPos, 1.f), g_matWVP);
    
    output.vViewPos = mul(float4(_in.vPos, 1.f), g_matWV).xyz;
    output.vViewTangent = normalize(mul(float4(_in.vTangent, 0.f), g_matWV)).xyz;
    output.vViewNormal = normalize(mul(float4(_in.vNormal, 0.f), g_matWV)).xyz;
    output.vViewBinormal = normalize(mul(float4(_in.vBinormal, 0.f), g_matWV)).xyz;

    output.vUV = _in.vUV;
    
    return output;
}


float4 PS_ColumnFlame(VTX_OUT _in) : SV_Target
{
    float4 vOutColor = (float4) 0.f;

    //_in.vUV.x *= 0.2f;
    _in.vUV.y += YPOS;
    
    if (g_btex_0)
    {
        vOutColor = g_tex_0.Sample(g_sam_0, _in.vUV);
    }
    
    vOutColor *= COLOR;

    if (0.1f > vOutColor.b)
        vOutColor.a = 0.f;

    return vOutColor;
}

// =========================
// Magma Shader
// DOMAIN : DOMAIN_DEFERRED
// Rasterizer : CULL_NONE
// DepthStencilState : LESS
// BlendState : DEFAULT
// =========================

#define EXPLODE_ON g_float_0

VTX_OUT VS_Magma(VTX_IN _in)
{
    VTX_OUT output = (VTX_OUT) 0.f;

    output.vPosition = mul(float4(_in.vPos, 1.f), g_matWVP);
    
    output.vViewPos = mul(float4(_in.vPos, 1.f), g_matWV).xyz;
    output.vViewTangent = normalize(mul(float4(_in.vTangent, 0.f), g_matWV)).xyz;
    output.vViewNormal = normalize(mul(float4(_in.vNormal, 0.f), g_matWV)).xyz;
    output.vViewBinormal = normalize(mul(float4(_in.vBinormal, 0.f), g_matWV)).xyz;

    output.vUV = _in.vUV;
    
    return output;
}


float4 PS_Magma(VTX_OUT _in) : SV_Target
{
    float4 vOutColor = (float4) 0.f;
    
    if (g_btex_0)
    {
        vOutColor = g_tex_0.Sample(g_sam_0, _in.vUV);
    }
    
    vOutColor += g_float_0 * vOutColor;
    

    return vOutColor;
}


// =========================
// Magma Shader
// DOMAIN : DOMAIN_DEFERRED
// Rasterizer : CULL_NONE
// DepthStencilState : LESS
// BlendState : DEFAULT
// g_tex_0 : Output Texture
// g_tex_1 : Normal Map
// g_tex_2 : Specular Map
// g_tex_3 : Embient Map
// g_int_0 : Hit State On
// =========================

VTX_OUT VS_Pot(VTX_IN _in)
{
    VTX_OUT output = (VTX_OUT) 0.f;
        
    if (g_iAnim)
    {
        Skinning(_in.vPos, _in.vTangent, _in.vBinormal, _in.vNormal, _in.vWeights, _in.vIndices, 0);
    }

    output.vPosition = mul(float4(_in.vPos, 1.f), g_matWVP);
    
    output.vViewPos = mul(float4(_in.vPos, 1.f), g_matWV).xyz;
    output.vViewTangent = normalize(mul(float4(_in.vTangent, 0.f), g_matWV)).xyz;
    output.vViewNormal = normalize(mul(float4(_in.vNormal, 0.f), g_matWV)).xyz;
    output.vViewBinormal = normalize(mul(float4(_in.vBinormal, 0.f), g_matWV)).xyz;
    output.vUV = _in.vUV;
    
    return output;
}

struct PS_OUT
{
    float4 vColor : SV_Target0;
    float4 vNormal : SV_Target1;
    float4 vPosition : SV_Target2;
    float4 vData : SV_Target3;

};

PS_OUT PS_Pot(VTX_OUT _in)
{
    PS_OUT output = (PS_OUT) 0.f;
    
    output.vColor = float4(1.f, 0.f, 1.f, 1.f);
    
    if (g_btex_0)
    {
        output.vColor = g_tex_0.Sample(g_sam_0, _in.vUV);        
    } 
    
    // 노말맵핑
    float3 vViewNormal = _in.vViewNormal;
    if (g_btex_1)
    {
        float3 vNormal = g_tex_1.Sample(g_sam_0, _in.vUV).rgb;
        vNormal = vNormal * 2.f - 1.f; // 0~1 값을 -1 ~ 1 로 확장       
        
        float3x3 matRot =
        {
            _in.vViewTangent
            , _in.vViewBinormal
            , _in.vViewNormal
        };
        
        vViewNormal = normalize(mul(vNormal, matRot));
    }

    // 반사맵
    if (g_btex_2)
    {
        float4 vSpec = g_tex_2.Sample(g_sam_0, _in.vUV);
        float fSpecData = encode(vSpec);
        output.vData.r = fSpecData;
    }
    
    if (1 == g_int_0)
    {
        output.vColor *= float4(1.f, 0.3f, 0.3f, 1.f);

    }
    
    return output;
}


// =========================
// Magma Shader
// DOMAIN : DOMAIN_DEFERRED
// Rasterizer : CULL_NONE
// DepthStencilState : LESS
// BlendState : DEFAULT
// g_tex_0 : Output Texture
// g_tex_1 : Emissive Target
// =========================

VTX_OUT VS_EnergyBall(VTX_IN _in)
{
    VTX_OUT output = (VTX_OUT) 0.f;

    output.vPosition = mul(float4(_in.vPos, 1.f), g_matWVP);
    
    output.vViewPos = mul(float4(_in.vPos, 1.f), g_matWV).xyz;
    output.vViewTangent = normalize(mul(float4(_in.vTangent, 0.f), g_matWV)).xyz;
    output.vViewNormal = normalize(mul(float4(_in.vNormal, 0.f), g_matWV)).xyz;
    output.vViewBinormal = normalize(mul(float4(_in.vBinormal, 0.f), g_matWV)).xyz;

    output.vUV = _in.vUV;
    
    return output;
}

float4 PS_EnergyBall(VTX_OUT _in) : SV_Target
{
    
    float4 output = (float4) 0.f;
    
    if (g_btex_0)
    {
        output = g_tex_0.Sample(g_sam_0, _in.vUV);

    }
    
    // 랜덤한 위치 부여
    float2 vSampleUV = float2(output.x + fAccTime, fAccTime);
    vSampleUV.y += sin(vSampleUV.x * 3.141592f * 10.f) * 0.1f;
    float3 vOut = GaussianSample(g_noise_cloud, vSampleUV).rgb;
                        
    float3 vRandom =
    {
        GaussianSample(g_noise_cloud, vSampleUV + float2(0.1f, 0.f)).r
        , GaussianSample(g_noise_cloud, vSampleUV + float2(0.2f, 0.f)).r
        , GaussianSample(g_noise_cloud, vSampleUV + float2(0.3f, 0.f)).r
    };
    
    float4 RandColor = (float4) 0.f;
    
    if (g_btex_0)
    {
        float2 RandUV = float2(_in.vUV.x + vOut.r, _in.vUV.y + vOut.r);
        RandColor = g_tex_0.Sample(g_sam_0, RandUV);
    }
    
    //vRandom.r = clamp(vRandom.r, 0.2f, 1.f);
    //vRandom.g = clamp(vRandom.g, 0.2f, 1.f);
    //vRandom.b = clamp(vRandom.b, 0.2f, 1.f);
    //vRandom = normalize(vRandom);
    //output += RandColor * output * 3.f;
     
    output.xyz = (RandColor.xyz * (vOut.r) * 3.f) + (output.xyz * (1.2f - vOut.r) * 1.f);
    
    
    return output;
}

#endif
