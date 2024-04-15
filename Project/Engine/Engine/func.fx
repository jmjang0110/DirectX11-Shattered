#ifndef _FUNC
#define _FUNC

#include "value.fx"

static float GaussianFilter[5][5] =
{
    0.003f, 0.0133f, 0.0219f, 0.0133f, 0.003f,
    0.0133f, 0.0596f, 0.0983f, 0.0596f, 0.0133f,
    0.0219f, 0.0983f, 0.1621f, 0.0983f, 0.0219f,
    0.0133f, 0.0596f, 0.0983f, 0.0596f, 0.0133f,
    0.003f, 0.0133f, 0.0219f, 0.0133f, 0.003f,
};

int IsBind(in Texture2D _tex)
{
    uint width = 0;
    uint height = 0;
    _tex.GetDimensions(width, height);
    
    if (0 != width || 0 != height)
        return 1;
   
    return 0;
}

float4 GaussianSample(in Texture2D _noisetex, float2 _vUV)
{
    float4 vOutColor = (float4) 0.f;
    
    if (0.f < _vUV.x)
    {
        _vUV.x = frac(_vUV.x);
    }
    
    if (0.f < _vUV.y)
    {
        _vUV.y = frac(_vUV.y);
    }
    
    // NoiseTexture 해상도를 이용해서 픽셀 인덱스(정수) 를 알아낸다.
    int2 iPixelIdx = (int2) (vNoiseResolution * _vUV);
    iPixelIdx -= int2(2, 2);
    
    
    // 해당 픽셀을 중심으로 가우시안 필터를 적용해서 색상의 평균치를 계산한다.
    for (int i = 0; i < 5; ++i)
    {
        for (int j = 0; j < 5; ++j)
        {
            int2 idx = int2(iPixelIdx.x + j, iPixelIdx.y + i);
            vOutColor += _noisetex[idx] * GaussianFilter[j][i];
        }
    }
    
    return vOutColor;
}


float3 CalculateLight2D(float3 _vWorldPos, float3 _vWorldNormal)
{
    float3 vLightColor = (float3) 0.f;
    float fLightPow = 1.f;
    //iLight2DCount;
    // Dir 0
    // Point 1
    // Spot 2
    
    for (int i = 0; i < iLight2DCount; ++i)
    {
        if (0 == g_Light2DBuffer[i].iLightType)
        {
            // 빛의 방향과 월드 노말의 내적(램버트 코사인법칙)
            fLightPow = saturate(dot(_vWorldNormal, g_Light2DBuffer[i].vLightDir));
            vLightColor += g_Light2DBuffer[i].color.vDiff.rgb * fLightPow;
        }
        else if (1 == g_Light2DBuffer[i].iLightType)
        {
            // pixel worldpos --> Light World Pos, Direction
            float3 vLightDir = normalize(_vWorldPos - g_Light2DBuffer[i].vWorldPos);
            fLightPow = saturate(dot(_vWorldNormal, vLightDir));
            
            // pixel worldpos <--> Light World Pos, distance
            float fDistance = distance(g_Light2DBuffer[i].vWorldPos.xy, _vWorldPos.xy);
            float fRatio = 1.f - saturate(fDistance / g_Light2DBuffer[i].fRange);
            vLightColor += g_Light2DBuffer[i].color.vDiff.rgb * fRatio * fLightPow;
        }
        else if (2 == g_Light2DBuffer[i].iLightType)
        {
            
        }
    }
    
    return vLightColor;

}

void CalculateLight3D(float3 _vViewPos, float3 _vViewNormal, int _iLight3DIdx, inout tLightColor _colorOut)
{
    tLightInfo info = g_Light3DBuffer[_iLight3DIdx];
    
    float fDiffusePower = 0.f;
    float fReflectPower = 0.f;
    float fRatio = 1.f; // 거리에 따른 빛의 세기 비율
    
    
    // Directional Light
    if (0 == info.iLightType)
    {
        // View LightDir
        float3 vViewLight = -normalize(mul(float4(info.vLightDir, 0.f), g_matView).xyz);
   
        // Diffuse Power
        fDiffusePower = saturate(dot(vViewLight, _vViewNormal));
    
        // 반사벡터
        float3 vViewReflect = -vViewLight + dot(vViewLight, _vViewNormal) * 2.f * _vViewNormal;
        vViewReflect = normalize(vViewReflect);
        //reflect(-vViewLight, _vViewNormal);    
        
        // 시선 벡터와 반사벡터를 내적해서 해당 표면의 반사광의 세기를 구한다.
        float3 vEye = -normalize(_vViewPos);
        fReflectPower = saturate(dot(vViewReflect, vEye));
        fReflectPower = pow(fReflectPower, 20);
    }
    
    // Point Light
    else if (1 == info.iLightType)
    {
        // View Light Position
        float3 vViewLightPos = mul(float4(info.vWorldPos, 1.f), g_matView).xyz;
        
        // ObjectPos - LightPos ==> LightDir
        float3 vViewLight = _vViewPos - vViewLightPos;
        float fDistance = length(vViewLight);
        vViewLight = -normalize(vViewLight);
        
        // Diffuse Power
        fDiffusePower = saturate(dot(vViewLight, _vViewNormal));
     
        // 반사벡터
        float3 vViewReflect = -vViewLight + dot(vViewLight, _vViewNormal) * 2.f * _vViewNormal;
        vViewReflect = normalize(vViewReflect);
        
        // 시선 벡터와 반사벡터를 내적해서 해당 표면의 반사광의 세기를 구한다.
        float3 vEye = -normalize(_vViewPos);
        fReflectPower = saturate(dot(vViewReflect, vEye));
        fReflectPower = pow(fReflectPower, 20);
        
        // 거리에 따른 빛의 감쇄 효과
        //fRatio = saturate(1.f - fDistance / info.fRange);
        fRatio = saturate(cos(saturate(fDistance / info.fRange) * (3.14159289f / 2.f)));
    }
    
    // Spot Light
    else
    {
        
    }
       
    
    _colorOut.vDiff += info.color.vDiff * fDiffusePower * fRatio;
    _colorOut.vSpec += info.color.vSpec * fReflectPower * fRatio;
    _colorOut.vAmb += info.color.vAmb;
}


void CalculateLight3D_Particle(float3 _vViewPos, int _iLight3DIdx, inout tLightColor _colorOut)
{
    tLightInfo info = g_Light3DBuffer[_iLight3DIdx];
    
    float fDiffusePower = 0.f;
    float fReflectPower = 0.f;
    float fRatio = 1.f; // 거리에 따른 빛의 세기 비율
        
    // Directional Light
    if (0 == info.iLightType)
    {
        // View LightDir
        float3 vViewLight = -normalize(mul(float4(info.vLightDir, 0.f), g_matView).xyz);
   
        // Diffuse Power
        fDiffusePower = 1.f;
    }
    
    // Point Light
    else if (1 == info.iLightType)
    {
        // View Light Position
        float3 vViewLightPos = mul(float4(info.vWorldPos, 1.f), g_matView).xyz;
        
        // ObjectPos - LightPos ==> LightDir
        float3 vViewLight = _vViewPos - vViewLightPos;
        float fDistance = length(vViewLight);
        
        // Diffuse Power
        fDiffusePower = 1.f;
             
        // 거리에 따른 빛의 감쇄 효과
        //fRatio = saturate(1.f - fDistance / info.fRange);
        fRatio = saturate(cos(saturate(fDistance / info.fRange) * (3.14159289f / 2.f)));
    }
    
    // Spot Light
    else
    {
        
    }
       
    _colorOut.vDiff += info.color.vDiff * fDiffusePower * fRatio;
    _colorOut.vSpec += info.color.vSpec * fReflectPower * fRatio;
    _colorOut.vAmb += info.color.vAmb;
}



// 카메라와 패치사이의 거리에 따른 테셀레이션 레벨 계산하기
float GetTessFactor(float3 _vViewPos, float _iMinLevel, float _iMaxLevel, float _fMaxDist, float _fMinDist)
{
    if (_iMinLevel > _iMaxLevel || _fMaxDist > _fMinDist)
        return 1.f;

    // 카메라와의 거리
    float fDist = length(_vViewPos);

    float fRatio = (fDist - _fMaxDist) / (_fMinDist - _fMaxDist);
    float fLevelStep = _iMaxLevel - _iMinLevel - 1.f;

    float fTessLv = _iMaxLevel - (fLevelStep * fRatio);

    return clamp(fTessLv, _iMinLevel, _iMaxLevel);
}



int IntersectsLay(float3 _vertices[3], float3 _vStart, float3 _vDir, out float3 _vCrossPoint, out float _fResult)
{
    float3 edge[2] = { (float3) 0.f, (float3) 0.f };
    edge[0] = _vertices[1].xyz - _vertices[0].xyz;
    edge[1] = _vertices[2].xyz - _vertices[0].xyz;

    float3 normal = normalize(cross(edge[0], edge[1]));
    float b = dot(normal, _vDir);

    float3 w0 = _vStart - _vertices[0].xyz;
    float a = -dot(normal, w0);
    float t = a / b;

    _fResult = t;

    float3 p = _vStart + t * _vDir;

    _vCrossPoint = p;

    float uu, uv, vv, wu, wv, inverseD;
    uu = dot(edge[0], edge[0]);
    uv = dot(edge[0], edge[1]);
    vv = dot(edge[1], edge[1]);

    float3 w = p - _vertices[0].xyz;
    wu = dot(w, edge[0]);
    wv = dot(w, edge[1]);
    inverseD = uv * uv - uu * vv;
    inverseD = 1.0f / inverseD;

    float u = (uv * wv - vv * wu) * inverseD;
    if (u < 0.0f || u > 1.0f)
    {
        return 0;
    }

    float v = (uv * wu - uu * wv) * inverseD;
    if (v < 0.0f || (u + v) > 1.0f)
    {
        return 0;
    }

    return 1;
}




matrix GetBoneMat(int _iBoneIdx, int _iRowIdx)
{
    return g_arrBoneMat[(g_iBoneCount * _iRowIdx) + _iBoneIdx];
}

void Skinning(inout float3 _vPos, inout float3 _vTangent, inout float3 _vBinormal, inout float3 _vNormal
    , inout float4 _vWeight, inout float4 _vIndices
    , int _iRowIdx)
{
    tSkinningInfo info = (tSkinningInfo) 0.f;

    if (_iRowIdx == -1)
        return;

    for (int i = 0; i < 4; ++i)
    {
        if (0.f == _vWeight[i])
            continue;

        matrix matBone = GetBoneMat((int) _vIndices[i], _iRowIdx);

        info.vPos += (mul(float4(_vPos, 1.f), matBone) * _vWeight[i]).xyz;
        info.vTangent += (mul(float4(_vTangent, 0.f), matBone) * _vWeight[i]).xyz;
        info.vBinormal += (mul(float4(_vBinormal, 0.f), matBone) * _vWeight[i]).xyz;
        info.vNormal += (mul(float4(_vNormal, 0.f), matBone) * _vWeight[i]).xyz;
    }

    _vPos = info.vPos;
    _vTangent = normalize(info.vTangent);
    _vBinormal = normalize(info.vBinormal);
    _vNormal = normalize(info.vNormal);
}


float encode(float4 _value)
{
    uint rgba = (uint(_value.x * 255.f) << 24) + (uint(_value.y * 255.f) << 16) + (uint(_value.z * 255.f) << 8) + uint(_value.w * 255.f);
    return asfloat(rgba);
}

float4 decode(float _value)
{
    uint rgba = asint(_value);

    float r = (float) (rgba >> 24) / 255.f;
    float g = (float) ((rgba & 0x00ff0000) >> 16) / 255.f;
    float b = (float) ((rgba & 0x0000ff00) >> 8) / 255.f;
    float a = (float) (rgba & 0x000000ff) / 255.f;

    return float4(r, g, b, a);
}



float4 PaperBurn(float4 vColor, float4 vBurnColor, float fNoise, float fStrength)
{
    
    float4 vOutColor = vColor;
    vOutColor.a -= ((fNoise * fStrength) + fStrength);
        
    float fRatio = 0.f;
    
    if (0.2f <= vOutColor.a)
    {
        fRatio = 1.f - ((vOutColor.a - 0.2f) / 0.8f);
        vOutColor.rgb = vBurnColor.rgb * (fRatio) + (vOutColor.rgb * (1.f - fRatio));
    }
    else if (0.05f < vOutColor.a)
    {
        fRatio = 1.f - ((vOutColor.a - 0.05f) / 0.15f);
        vOutColor.rgb = float3(1.f, 1.f, 1.f) * (fRatio) + (vBurnColor.rgb * (1.f - fRatio));
    }
    else
    {
        fRatio = 1.f - ((vOutColor.a - 0.f) / 0.05f);
        vOutColor.rgb = float3(0.f, 0.f, 0.f) * (fRatio) + (float3(1.f, 1.f, 1.f) * (1.f - fRatio));
    }
      
    return vOutColor;
    
}

#endif