#ifndef _LIGHT
#define _LIGHT

#include "value.fx"


// ========================
// Directional Light Shader
// MRT      : Light
// mesh     : RectMesh
// RS       : CULL_BACK
// DS       : NO_TEST_NO_WRITE
// BS       : ONE_ONE

#define LightIdx            g_int_0
#define PositionTarget      g_tex_0
#define NormalTarget        g_tex_1
#define ShadowMap           g_tex_2
#define DataTarget          g_tex_3

#define LightVP         g_mat_0
// ========================

struct VS_DIR_IN
{
    float3 vPos : POSITION;
};

struct VS_DIR_OUT
{
    float4 vPosition : SV_Position;
};

VS_DIR_OUT VS_Directional(VS_DIR_IN _in)
{
    VS_DIR_OUT output = (VS_DIR_OUT) 0.f;

    output.vPosition = float4(_in.vPos * 2.f, 1.f);
    
    return output;
}

struct PS_DIR_OUT
{
    float4 vDiffuse : SV_Target0;
    float4 vSpecular : SV_Target1;
    float4 vShadowPow : SV_Target2;
};

PS_DIR_OUT PS_Directional(VS_DIR_OUT _in)
{
    PS_DIR_OUT output = (PS_DIR_OUT) 0.f;
    
    float2 vUV = _in.vPosition.xy / vResolution.xy;
    
    float3 vViewPos = PositionTarget.Sample(g_sam_0, vUV).xyz;
    float3 vViewNormal = NormalTarget.Sample(g_sam_0, vUV).xyz;
    float fShadowPow = 0.f;
    
    // 물체가 그려지지 않은 영역은 빛을 받을 수 없다.
    if (vViewPos.z == 0.f)
        clip(-1);
        
    tLightColor color = (tLightColor) 0.f;
    CalculateLight3D(vViewPos, vViewNormal, LightIdx, color);
        
    // 그림자 판정
    // ViewPos -> WorldPos
    float3 vWorldPos = mul(float4(vViewPos, 1.f), g_matViewInv).xyz;
    
    // WorldPos -> Light 투영
    float4 vLightProj = mul(float4(vWorldPos, 1.f), LightVP);
    
    // w 로 나눠서 실제 xy 투영좌표를 구함
    vLightProj.xyz /= vLightProj.w;
    
    // 샘플링을 하기 위해서 투영좌표계를 UV 좌표계로 변환
    float2 vShadowMapUV = float2((vLightProj.x / 2.f) + 0.5f, -(vLightProj.y / 2.f) + 0.5f);
    float fShadowMapDepth = ShadowMap.Sample(g_sam_0, vShadowMapUV).r;
    
    // 광원에 기록된 깊이보다, 물체의 깊이가 더 멀 때, 그림자 판정
    if (0.f != fShadowMapDepth
        && 0.f <= vShadowMapUV.x && vShadowMapUV.x <= 1.f
        && 0.f <= vShadowMapUV.y && vShadowMapUV.y <= 1.f
        && vLightProj.z >= fShadowMapDepth + 0.0001f)
    {
        fShadowPow = 0.6f;
    }
    
    float fSpecData = DataTarget.Sample(g_sam_0, vUV).r;
    if (0.f != fSpecData)
    {
        float3 vSpecCoeff = decode(fSpecData.r).rgb;
        color.vSpec.rgb *= vSpecCoeff;
        //color.vDiff.rgb *= vSpecCoeff;
        //color.vAmb.rgb *= vSpecCoeff;
    }

    output.vDiffuse = (color.vDiff + color.vAmb);
    output.vSpecular = color.vSpec;
    output.vShadowPow = fShadowPow;
    
    output.vDiffuse.a = 1.f;
    output.vSpecular.a = 1.f;
    output.vShadowPow.a = 1.f;
        
    return output;
}



// ==================
// Point Light Shader
// MRT      : Light
// Mesh     : Sphere
// RS_TYPE  : CULL_FRONT, 광원영역(Volume Mesh) 안으로 들어왔을 때를 대비
// BS_TYPE  : ONE_ONE, 기존에 그려진 빛(타겟) 을 누적
// DS_TYPE  : NO_TEST_NO_WRITE

//#define LightIdx        g_int_0
//#define PositionTarget  g_tex_0
//#define NormalTarget    g_tex_1
// ==================
VS_DIR_OUT VS_Point(VS_DIR_IN _in)
{
    VS_DIR_OUT output = (VS_DIR_OUT) 0.f;

    output.vPosition = mul(float4(_in.vPos, 1.f), g_matWVP);
        
    return output;
}


PS_DIR_OUT PS_Point(VS_DIR_OUT _in)
{
    PS_DIR_OUT output = (PS_DIR_OUT) 0.f;
        
    float2 vUV = _in.vPosition.xy / vResolution.xy;
    float3 vViewNormal = NormalTarget.Sample(g_sam_0, vUV).xyz;
        
    float3 vViewPos = PositionTarget.Sample(g_sam_0, vUV).xyz;
    float3 vWorldPos = mul(float4(vViewPos, 1.f), g_matViewInv).xyz;
    float3 vLocalPos = mul(float4(vWorldPos, 1.f), g_matWorldInv).xyz;
    
    if (length(vLocalPos) < 0.5f)
    {
        tLightColor color = (tLightColor) 0.f;
        CalculateLight3D(vViewPos, vViewNormal, LightIdx, color);
                      
        output.vDiffuse = color.vDiff + color.vAmb;
        output.vSpecular = color.vSpec;
        output.vShadowPow = (float4) 0.f;
    
        output.vDiffuse.a = 1.f;
        output.vSpecular.a = 1.f;
        output.vShadowPow.a = 1.f;
    }
    else
    {
        clip(-1);
        //output.vDiffuse = float4(1.f, 0.f, 0.f, 1.f);
    }
         
    return output;
}

// =================
// Spot Light Shader
// 
// MRT      : Light
// Mesh     : Cone
// RS_TYPE  : CULL_FRONT, 광원영역(Volume Mesh) 안으로 들어왔을 때를 대비
// BS_TYPE  : ONE_ONE, 기존에 그려진 빛(타겟) 을 누적
// DS_TYPE  : NO_TEST_NO_WRITE
// =================

VS_DIR_OUT VS_Spot(VS_DIR_IN _in)
{
    VS_DIR_OUT output = (VS_DIR_OUT) 0.f;

    output.vPosition = mul(float4(_in.vPos, 1.f), g_matWVP);

    return output;
}


PS_DIR_OUT PS_Spot(VS_DIR_OUT _in)
{
    PS_DIR_OUT output = (PS_DIR_OUT) 0.f;

    float2 vUV = _in.vPosition.xy / vResolution.xy;
    float3 vViewNormal = NormalTarget.Sample(g_sam_0, vUV).xyz;

    float3 vViewPos = PositionTarget.Sample(g_sam_0, vUV).xyz;
    float3 vWorldPos = mul(float4(vViewPos, 1.f), g_matViewInv).xyz;
    float3 vLocalPos = mul(float4(vWorldPos, 1.f), g_matWorldInv).xyz;

    if (-0.5f < vLocalPos.z && vLocalPos.z < 0.5f)
    {
        tLightColor color = (tLightColor) 0.f;
        CalculateLight3D(vViewPos, vViewNormal, LightIdx, color);

        float height = 0.f;

        if (vLocalPos.z < 0)
            height = (0.5f + vLocalPos.z);
        else
            height += vLocalPos.z + 0.5f;

        float2 xyLength = vLocalPos.xy;
        if (height / 2.f > length(xyLength))
        {
            output.vDiffuse = color.vDiff + color.vAmb;
            output.vSpecular = color.vSpec;
            output.vShadowPow = (float4) 0.f;

            output.vDiffuse.a = 1.f;
            output.vSpecular.a = 1.f;
            output.vShadowPow.a = 1.f;
        }
        else
            clip(-1);
    }
    else
    {
        clip(-1);
    }


    return output;
}


// ==============================
// Merge Shader
// Mesh : RectMesh
#define ColorTarget             g_tex_0
#define DiffuseTarget           g_tex_1
#define SpecularTarget          g_tex_2
#define ShadowPowTarget         g_tex_3
#define AmbientOcclusionTarget  g_tex_4
#define EmissiveTarget          g_tex_5
// ==============================
struct VS_MERGE_IN
{
    float3 vPos : POSITION;
};

struct VS_MERGE_OUT
{
    float4 vPosition : SV_Position;
};

static const float Weight[9] = // 4 - 1 - 4
{
    0.1f, 0.18f, 0.55f, 0.9f,
    1, 0.9f, 0.55f, 0.18f, 0.1f
};

float4 ApplySSAO(float _fAOaccess)
{
    
    float4 vAmbient = float4(0.0f, 0.0f, 0.0f, 0.0f);
    float4 vPhongMat = float4(0.1f, 0.1f, 0.1f, 1.f);
    float4 vDirLightAmb = float4(0.2f, 0.2f, 0.2f, 1.f);

    float4 vAmb = (vPhongMat * vDirLightAmb * _fAOaccess) * 5;
    
    if (_fAOaccess >= 0.95f)
    {
        float fStrength = vAmb.a * 0.05f;
        vAmb = float4(fStrength, fStrength, fStrength, fStrength);

    }
    else
        vAmb.xyz *= -1.3f;
    
    return vAmb;
}

float4 ApplyBloom(float4 vColor, float2 _vUV)
{
    float4 vOutColor = vColor;
    
    // bloom
    float fWeightOnePixel = 1.f / vResolution.x;
    float fHeightOnePixel = 1.f / vResolution.y;

    float2 t = _vUV;
    float2 uv = (float2) 0.f;
    float4 vBloomColor = (float4) 0.f;

    for (int i = -4; i < 4; ++i)
    {
        uv = t + float2(fWeightOnePixel * i, 0.f);
        vBloomColor += Weight[4 + i] * g_tex_0.Sample(g_sam_0, uv);
    }

    for (int j = -4; j < 4; ++j)
    {
        uv = t + float2(0, fHeightOnePixel * j);
        vBloomColor += Weight[4 + j] * g_tex_0.Sample(g_sam_0, uv);
    }

    // bloom 추출
    vBloomColor /= 8.f;
    float vBrightness = dot(vBloomColor.rgb, float3(0.2126f, 0.7152f, 0.0722f));

    if (vBrightness > 0.99)
    {
        return vBloomColor;
    }
    else
        return vOutColor;
    
}

VS_MERGE_OUT VS_Merge(VS_MERGE_IN _in)
{
    VS_MERGE_OUT output = (VS_MERGE_OUT) 0.f;

    output.vPosition = float4(_in.vPos * 2.f, 1.f);
    
    return output;
}

float4 PS_Merge(VS_MERGE_OUT _in) : SV_Target0
{
    float4 vOutColor = (float4) 0.f;
    
    float2 vUV = _in.vPosition.xy / vResolution.xy;
    
    float4 vColor           = ColorTarget.Sample(g_sam_0, vUV);
    float4 vDiffuse         = DiffuseTarget.Sample(g_sam_0, vUV);
    float4 vSpecular        = SpecularTarget.Sample(g_sam_0, vUV);
    float4 vAO              = AmbientOcclusionTarget.Sample(g_sam_0, vUV);
    float4 vEmissive        = EmissiveTarget.Sample(g_sam_0, vUV);

    float fShadowPow        = ShadowPowTarget.Sample(g_sam_0, vUV).x;
    float fAmbientAccess    = AmbientOcclusionTarget.Sample(g_sam_0, vUV).r;
    
    // SSAO 
    float4 vAmb = ApplySSAO(fAmbientAccess);
    
    // vDiffuse 에 이미 Ambient 가 더해져있음  
    vOutColor = float4(vColor.xyz * (vAmb.xyz + vDiffuse.xyz) * (1.f - fShadowPow)
                        + vSpecular.xyz * (1.f - fShadowPow), 1.f);
    vOutColor.a = vColor.a * vDiffuse.a;
    
    // BLOOM
    vOutColor = ApplyBloom(vOutColor, vUV);
    return vOutColor;
}



// ===============
// ShdowMap Shader
// MRT : ShadowMap MRT
// RS : CULL_BACK
// BS : Default
// DS : Less
// ===============
struct VS_SHADOW_IN
{
    float3 vPos : POSITION;
    float2 vUV : TEXCOORD;

    float3 vTangent : TANGENT;
    float3 vNormal : NORMAL;
    float3 vBinormal : BINORMAL;

    float4 vWeights : BLENDWEIGHT;
    float4 vIndices : BLENDINDICES;
};

struct VS_SHADOW_OUT
{
    float4 vPosition : SV_Position;
    float4 vProjPos : POSITION;
};

VS_SHADOW_OUT VS_ShadowMap(VS_SHADOW_IN _in)
{
    VS_SHADOW_OUT output = (VS_SHADOW_OUT) 0.f;

    if (g_iAnim)
    {
        Skinning(_in.vPos, _in.vTangent, _in.vBinormal, _in.vNormal, _in.vWeights, _in.vIndices, 0);
    }
    
    output.vPosition = mul(float4(_in.vPos, 1.f), g_matWVP);
    output.vProjPos = output.vPosition;
    
    return output;
}

float PS_ShadowMap(VS_SHADOW_OUT _in) : SV_Target
{
    float fOut = 0.f;
    fOut = _in.vProjPos.z / _in.vProjPos.w;
    return fOut;
}





#endif